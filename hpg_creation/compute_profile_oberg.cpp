// ==============================================================================
// ICAP License
// ==============================================================================
// University of Illinois/NCSA
// Open Source License
// 
// Copyright (c) 2014-2016 University of Illinois at Urbana-Champaign.
// All rights reserved.
// 
// Developed by:
// 
//     Nils Oberg
//     Blake J. Landry, PhD
//     Arthur R. Schmidt, PhD
//     Ven Te Chow Hydrosystems Lab
// 
//     University of Illinois at Urbana-Champaign
// 
//     https://vtchl.illinois.edu
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal with
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimers.
// 
//     * Redistributions in binary form must reproduce the above copyright notice,
//       this list of conditions and the following disclaimers in the
//       documentation and/or other materials provided with the distribution.
// 
//     * Neither the names of the Ven Te Chow Hydrosystems Lab, University of
// 	  Illinois at Urbana-Champaign, nor the names of its contributors may be
// 	  used to endorse or promote products derived from this Software without
// 	  specific prior written permission.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
// SOFTWARE.


#include <numeric>
#include <limits>
#include <algorithm>

#include "../hpg/error.hpp"
#include "../xslib/cross_section.h"
#include "../util/math.h"

#include "hpg_creator.hpp"
#include "mannings_math.h"
#include "normcrit.h"

#define SOL_TOL 1e-6



struct solver_params
{
    int maxIter, curIter;
};


struct profile_params : solver_params
{
    double D, L, N, S, Q, ks, G;
    double D_sqrd, Q_sqrd, N_sqrd, ks_sqrd;
    double first_area;
    double dx, curZ;
    double E, E_last, Sf, Sf_last, A, P, T, theta, V;
    int count;
    int isSteep;
    double forcing_factor;
};

int profile_error_code;
#define ZERO_AREA 399

inline double profile_func(double y, solver_params* params_in);
inline double profile_func_deriv(double y, solver_params* params_in);
inline void compute_variables(double y, profile_params* params);

/*
// This is an attempt to be more modular...
double ComputeProfile(double diameter, double length, double slope, double roughness,
    double flow, double yInit, int numComputations, bool isSteep, double& volume, double& hf_reach)
{
    profile_params params;
    profile_error_code = 0;

    // These are inherited from brent_params
    params.curIter = 0;
    params.maxIter = 50;

    params.D = diameter;
    params.L = length;
    params.N = roughness;
    params.S = slope;
    params.Q = flow;
    params.ks = KS;
    params.G = G;

    // Precompute to save computation time.
    params.D_sqrd = diameter * diameter;
    params.Q_sqrd = flow * flow;
    params.N_sqrd = roughness * roughness;
    params.ks_sqrd = KS * KS;
    params.isSteep = (isSteep ? -1 : 1); // this is a factor so we can use the same code for computing steep/mild

    params.first_area = -1.0;
    params.dx = length / numComputations;
    if (isSteep)
        params.dx = -params.dx; // make the x-increment negative if it's steep so we can use the same code for computing steep/mild

    double volumeArea = 0.0, lastArea = 0.0;

    double curX = 0.0, curZ = 0.0, startZ = 0.0;
    if (isSteep)
        curX = length; // start in the x-direction from the top of the reach if the reach is steep
    if (isSteep || slope < 0.0)
        startZ = curZ = length*fabs(slope); // start in the z-direction from the top of the reach if the reach is steep or adverse
    params.curZ = curZ;

    // We need to compute E_last and Sf_last.
    compute_variables(yInit, &params);
    if (profile_error_code)
    {
        ErrorCode = hpgns::error::at_min_depth;
        return hpgns::error::bad_value;
    }

    params.E_last = params.E;
    params.Sf_last = params.Sf;

    hf_reach = 0;

    // We need to pre-cast the pointer so we can make things cleaner later.
    solver_params* params_ptr = (solver_params*)&params;

    double yComp = yInit;
    double yMax = diameter * UnsteadyDepth;
    double fn, dfn;

    // This loop just saves the result of the last step.  We could instead
    // save all of the steps, effectively giving us the water surface profile.
    // However, we elect not to do that.
    int i = 0;
    int iterCount = 0;
    for (i = 0; i < numComputations; i++)
    {
        curX += params.dx;
        curZ += slope * params.dx;
        params.curZ = curZ;

        ErrorCode = 0;
        profile_error_code = 0;
        fn = 1.0;
        double yComp_last = yComp;
        iterCount = 0;

        // This is the actual solver for the given upstream point.  We need
        // to iterate until the solution converges or we've reached the max
        // number of iterations (divergence).
        while (fabs(fn) > ConvergenceFactor && iterCount < params.maxIter)
        {
            // These functions contain the equations and math for the solution.
            fn = profile_func(yComp, params_ptr);
            dfn = profile_func_deriv(yComp, params_ptr);

            yComp = yComp - fn / dfn;

            // If the solution went past the max depth, then stop.
            if (yComp > yMax)
            {
                ErrorCode = hpgns::error::at_max_depth;
                break;
            }
            // If the solution went to zero or negative depth, then stop.
            else if (yComp < ConvergenceFactor || profile_error_code == ZERO_AREA)
            {
                ErrorCode = hpgns::error::at_min_depth;
                break;
            }
            // If there was another error (imaginary solution) then stop.
            else if (profile_error_code)
            {
                break;
            }

            iterCount++;
        }

        double sfAvg = (params.Sf + params.Sf_last) * 0.5;
        hf_reach += sfAvg * fabs(params.dx);

        // Save the results of this step for the next step.
        params.E_last = params.E;
        params.Sf_last = params.Sf;
        lastArea = params.A;
        volumeArea += params.A;

        if (profile_error_code)
        {
            ErrorCode = profile_error_code;
            break;
        }
        else if (ErrorCode)
        {
            break;
        }
        else if (params.curIter >= params.maxIter)
        {
            ErrorCode = hpgns::error::divergence;
            break;
        }
    }

    // Compute the volume (the sum of the areas of the cross section
    // at each step minus the average of the first and last cross
    // section areas times the x-step).
    volume = (volumeArea - (lastArea + params.first_area) / 2.0) * fabs(params.dx);

    // If we terminated early, then return an error.
    if (i < numComputations && ErrorCode == 0)
    {
        ErrorCode = hpgns::error::at_max_depth;
        return hpgns::error::bad_value;
    }

    // If there was an error, then return it.
    else if (ErrorCode != 0)
        return hpgns::error::bad_value;

    // Otherwise we successfully solved for a point.
    else
    {
        return yComp;
    }
}


// This function computes the variables (theta, wetted perimeter, etc.)
// for later on.
inline void compute_variables(double y, profile_params* params)
{
    double acos_temp = 1.0 - 2.0 * y / params->D;
    if (acos_temp < -1.0 || acos_temp > 1.0)
    {
        profile_error_code = hpgns::error::imaginary;
    }

    double theta = 2.0 * acos(acos_temp);
    params->theta = theta;
    double A = 0.125 * (theta - sin(theta)) * params->D_sqrd;
    params->A = A;
    if (fabs(A) < 0.0000001) // is it zero?
    {
        profile_error_code = ZERO_AREA;
    }

    double P = 0.5 * theta * params->D;
    params->P = P;
    double T = params->D * sin(theta / 2.0);
    params->T = T;

    double V = params->Q / A;
    params->V = V;

    params->E = params->curZ + y + params->Q_sqrd / (A*A) / (2.0 * params->G);

    params->Sf = (params->Q * params->N * pow(P, 2.0 / 3.0)) / (params->ks * pow(A, 5.0 / 3.0));
    params->Sf = params->Sf * params->Sf; // square it

    if (fabs(params->first_area + 1.0) < 0.0000001)
        params->first_area = A;
}


// This the function we're attempting to find the root for.
// This works for both steep, mild, and adverse.
inline double profile_func(double y, solver_params* params_in)
{
    profile_params* params = (profile_params*)params_in;
    compute_variables(y, params);

    double fn = (params->E_last - params->E) * params->isSteep +
        (params->Sf_last + params->Sf) / 2.0 * fabs(params->dx);

    return fn;
}


// This is the derivative of the function we're attempting to find the
// root for.  This works for both steep, mild, and adverse.
inline double profile_func_deriv(double y, solver_params* params_in)
{
    profile_params* params = (profile_params*)params_in;

    double Q_sqrd = params->Q_sqrd;
    double G = params->G;
    double A = params->A;
    double dx = fabs(params->dx);
    double N_sqrd = params->N_sqrd;
    double ks_sqrd = params->ks_sqrd;
    double P = params->P;
    double T = params->T;
    double D = params->D;

    double dfn = -1.0 + Q_sqrd * T / (G * A*A*A) + (dx / 2.0) * (Q_sqrd * N_sqrd) / ks_sqrd *
        ((-10.0 / 3.0) * pow(P, 4.0 / 3.0) * pow(A, -13.0 / 3.0) * T + (8.0 / 3.0) * pow(A, -10.0 / 3.0) * pow(P, -1.0 / 3.0) *
        pow(1.0 - (1.0 - 2.0*y / D)*(1.0 - 2.0*y / D), -0.5));
    dfn = dfn * params->isSteep;

    return dfn;
}

*/
