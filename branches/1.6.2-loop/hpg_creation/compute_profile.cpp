
#include <numeric>
#include <limits>
#include <algorithm>

#include "../hpg/error.hpp"
#include "../xslib/cross_section.h"
#include "../util/math.h"

#include "hpg_creator.hpp"
#include "mannings_math.h"
#include "normcrit.h"

#define SOL_TOL 1e-4



struct solver_params
{
    double L, N, S, Q, kn, g, dx;
    int count;
    std::shared_ptr<xs::CrossSection> xs;
    double first_area;
    int isSteep;
    bool isFull;
    bool isEmpty;
    int maxIter, curIter;
};


struct profile_params
{
    double Y, Z, Sf, A, P, T, V, PoG;
};

int profile_error_code;
#define ERR_ZERO_AREA 399

inline double profile_func(double y, solver_params& params, profile_params& x1, profile_params& x2);
inline double profile_func_deriv(double y, solver_params& params, profile_params& x);
inline void compute_variables(double y, solver_params& params, profile_params& x);



// This code is based on the following code:
//      JM Mier, October 2010, UIUC
//      Based on previous version by JM Mier, November 2007, UIUC
//      JM Mier, UIUC, September 2013 - Modified to include English units

int ComputeProfile(const xs::Reach& reach, double flow, double yInit, int nC, bool isSteep, bool reverseSlope, bool freeOnly, double g, double kn, double maxDepthFrac, double& yUp, double& volume, double& hf_reach);

int ComputeFreeProfile(const xs::Reach& reach, double flow, double yInit, int nC, bool isSteep, bool reverseSlope, double g, double kn, double maxDepthFrac, double& yUp, double& volume, double& hf_reach)
{
    return ComputeProfile(reach, flow, yInit, nC, isSteep, reverseSlope, true, g, kn, maxDepthFrac, yUp, volume, hf_reach);
}

int ComputeCombinedProfile(const xs::Reach& reach, double flow, double yInit, int nC, bool isSteep, bool reverseSlope, double g, double kn, double maxDepthFrac, double& yUp, double& volume, double& hf_reach)
{
    return ComputeProfile(reach, flow, yInit, nC, isSteep, reverseSlope, false, g, kn, maxDepthFrac, yUp, volume, hf_reach);
}

int ComputeProfile(const xs::Reach& reach, double flow, double yInit, int nC, bool isSteep, bool reverseSlope, bool freeOnly, double g, double kn, double maxDepthFrac, double& yUp, double& volume, double& hf_reach)
{
    // Initialization
	hf_reach = 0;
    volume = 0;
    profile_error_code = 0;

    // Shortcuts for reach properties.
    double slope = reach.getSlope() * (reverseSlope ? -1. : 1.);
    double length = reach.getLength();
    double maxDepth = reach.getMaxDepth() * maxDepthFrac;

    //// If the flow is zero, assume a level-pool upstream elevation.
    //if (isZero(flow))
    //{
    //    yUp = std::max(0., yInit - slope * length);
    //}


    solver_params params;
    params.xs = reach.getXs();
    params.curIter = 0;
    params.maxIter = 50;
    params.L = length;
    params.N = reach.getRoughness();
    params.S = slope;
    params.Q = flow;
    params.kn = kn;
    params.g = g;
    params.isSteep = (isSteep ? -1 : 1); // this is a factor so we can use the same code for computing steep/mild
    params.first_area = -1.0;
    params.dx = length / nC;
    if (isSteep)
        params.dx = -params.dx; // make the x-increment negative if it's steep so we can use the same code for computing steep/mild

    profile_params x1;
    double volumeArea = 0.0, lastArea = 0.0;
    double curX = 0.0;
    x1.Z = 0.0;
    if (isSteep)
        curX = length; // start in the x-direction from the top of the reach if the reach is steep
    if (isSteep || slope < 0.0)
        x1.Z = length*fabs(slope); // start in the z-direction from the top of the reach if the reach is steep or adverse

    ///////////////////////////////////////////////////////////////////////////
    // Create profile variables
    using namespace std;
    double* X_ = new double[nC + 1];
    double* Y_ = new double[nC + 1];
    double* Z_ = new double[nC + 1];
    double* V_ = new double[nC + 1];
    double* Sf_ = new double[nC + 1];
    double* PoG_ = new double[nC + 1];
    double* H_ = new double[nC + 1];
    bool* e2_ = new bool[nC + 1];
    bool* p2_ = new bool[nC + 1];

    std::fill(X_, X_ + nC + 1, 0.);
    std::fill(Y_, Y_ + nC + 1, 0.);
    std::fill(Z_, Z_ + nC + 1, 0.);
    std::fill(V_, V_ + nC + 1, 0.);
    std::fill(Sf_, Sf_ + nC + 1, 0.);
    std::fill(PoG_, PoG_ + nC + 1, 0.);
    std::fill(H_, H_ + nC + 1, 0.);
    std::fill(e2_, e2_ + nC + 1, false);
    std::fill(p2_, p2_ + nC + 1, false);

    unique_ptr<double[]> X(X_);
    unique_ptr<double[]> Y(Y_);
    unique_ptr<double[]> Z(Z_);
    unique_ptr<double[]> V(V_);
    unique_ptr<double[]> Sf(Sf_);
    unique_ptr<double[]> PoG(PoG_);
    unique_ptr<double[]> H(H_);
    unique_ptr<bool[]> e2(e2_);
    unique_ptr<bool[]> p2(p2_);

    ///////////////////////////////////////////////////////////////////////////
    // Perform computations at downstream end of conduit.
    
    bool isFull = false;
    bool isEmpty = false;

    double y_c;
    bool yCvalid = true;
    if (ComputeCriticalDepth(reach, flow, g, y_c))
    {
        yCvalid = false;
    }

    double y_n;
    bool yNvalid = true;
    if (ComputeNormalDepth(reach, flow, g, kn, y_n))
    {
        yNvalid = false;
    }
    
    X[0] = 0;
    if (!isZero(flow))
        Y[0] = std::max(y_c, yInit);
    else
        Y[0] = yInit;
    //else
    //    Y[0] = yInit;
    Z[0] = x1.Z;

    // Starting out empty case
    if (Y[0] <= 0.0001 * maxDepth)
    {
        isEmpty = true;
        compute_variables(Y[0], params, x1);
        Y[0] = 0;
        Sf[0] = slope;
        PoG[0] = 0;
        V[0] = 0;
    }
    
    // Starting out full case
    else if (!isZero(flow) && Y[0] >= 0.9999 * maxDepth)
    {
        isFull = true;
        Y[0] = maxDepth;
        // Compute P, T, A, etc.
        compute_variables(Y[0], params, x1);
        PoG[0] = std::max(std::min(0., y_c - maxDepth), yInit - maxDepth);
        Sf[0] = x1.Sf;
        V[0] = x1.V;
        if (freeOnly)
        {
            return hpg::error::at_max_depth;
        }
    }

    // Free-surface case
    else
    {
        // Compute P, T, A, etc.
        compute_variables(Y[0], params, x1);
        PoG[0] = std::max(0., Y[0] - maxDepth);
        Sf[0] = x1.Sf;
        V[0] = x1.V;
    }

    // If the area is zero, then the reach is empty and we return empty.
    if (!isEmpty && profile_error_code == ERR_ZERO_AREA)
    {
        isEmpty = true;
        compute_variables(Y[0], params, x1);
        Y[0] = 0;
        Sf[0] = slope;
        PoG[0] = 0;
        V[0] = 0;
    }

    H[0] = Z[0] + Y[0] + PoG[0] + V[0] * V[0] / (2. * g);

    // Standard-step method.
    // Sub-index of 2 is the unknown; Sub-index of 1 is the current known.
    double y2, Sf2, V2;
    double y2k = Y[0]; // initial guess
    profile_params x2;
    int error = 0;

    if (Y[0] >= 0.98 * maxDepth)
    {
        isFull = true;
    }

    int i = 0;
    for (i = 1; i < nC + 1; i++)
    {
        curX += params.dx;

        X[i] = X[i - 1] + params.dx;
        Z[i] = Z[0] + slope * (X[i] - X[0]);
        x2.Z = Z[i];

        // If the flow is zero we simply use this to compute the volume
        if (isZero(flow))
        {
            y2 = std::max(0., yInit - curX * slope);
            compute_variables(y2, params, x2);
            Y[i] = y2;
            V[i] = 0;
            Sf[i] = 0;
            lastArea = x2.A;
            volumeArea += x2.A;
            continue;
        }

        else if (isEmpty)
        {
            compute_variables(0.0001 * maxDepth, params, x2);
            y2 = 0;
            Sf2 = slope;
            V2 = 0;
        }

        // If the previous section is pressurized, it stays pressurized upstream.
        else if (isFull)
        {
            compute_variables(maxDepth, params, x2);
            y2 = maxDepth;
            Sf2 = x2.Sf;
            V2 = x2.V;
            if (freeOnly)
            {
                return hpg::error::at_max_depth;
            }
        }

        // Otherwise, the previous section was not pressurized.
        else
        {
            // Y can't get higher than the max height in conduit
            // or Y can't get smaller than the min depth in conduit (included to avoid having A2=0 when Y=0, which gives problems later on due to division by 0)
            y2 = std::max(std::min(Y[i - 1], 0.9999 * maxDepth), 0.0001 * maxDepth);
            compute_variables(y2, params, x1);
            x1.Z = x2.Z - slope * params.dx;
            x1.PoG = PoG[i - 1];

            if (!e2[i] && !p2[i])
            {
                y2k = y2 + 0.01 * (y_n - y2);
            }
            y2k = std::max(std::min(y2k, 0.9999 * maxDepth), 0.0001 * maxDepth);

            double fn = 1.0;
            int iterCount = 0;

            // This is the actual solver for the given upstream point.  We need
            // to iterate until the solution converges or we've reached the max
            // number of iterations (divergence).
            bool converged = false;
            bool solutionNotChanging = false;
            while (!converged && iterCount < params.maxIter)
            {
                // These functions contain the equations and math for the solution.
                fn = profile_func(y2k, params, x1, x2);
                double dfn = profile_func_deriv(y2k, params, x2);

                double y2kp1 = y2k - fn/dfn;

                solutionNotChanging = false;
                if (std::abs(y2kp1 - y2k) / (0.5 * std::abs(y2kp1 + y2k)) < SOL_TOL)
                {
                    converged = true;
                    y2 = y2kp1;
                }
                else
                {
                    iterCount++;
                    double y2km1 = y2k;
                    // Bound the estimate by the pipe empty/full values.
                    y2k = std::max(std::min(y2kp1, 0.9999 * maxDepth), 0.0001 * maxDepth);

                    // Exit statement to avoid loop the same condition; this is useful when there is a change
                    // from non-pressurized to pressurized or empty.
                    if (isZero(y2km1 - y2k))
                    {
                        solutionNotChanging = true;
                        break;
                    }
                }
            }

            if (iterCount >= params.maxIter || solutionNotChanging)
            {
                y2 = y_n; // **THIS CONDITION WILL HAVE TO CHANGE WHEN THE SUPERCRITICAL LOGIC IS FULLY IMPLEMENTED**
                compute_variables(y_n, params, x2);
                Sf2 = x2.Sf;
                V2 = x1.V;
            }

            // If the conduit gets empty in this section, relcalculate for empty.
            if (y2 <= 0.0001 * maxDepth && !e2[i])
            {
                isEmpty = true;
                y2 = 0;
                Sf2 = slope;
                V2 = 0;
            }

            // If the flow gets pressurized in this section, recalculate for pressurized conditions.
            else if (y2 >= 0.9999 * maxDepth && !p2[i])
            {
                isFull = true;
                y2 = maxDepth;
                compute_variables(maxDepth, params, x2);
                Sf2 = x2.Sf;
                V2 = x2.V;
                if (freeOnly)
                {
                    return hpg::error::at_max_depth;
                }
            }
            else
            {
                Sf2 = x2.Sf;
                V2 = x2.V;
            }

            if (profile_error_code)
            {
                error = profile_error_code;
                break;
            }
        }

        // Once Sf1 and Y1 are obtained, compute hydraulic variables for the section
        Y[i] = y2;
        V[i] = V2;
        Sf[i] = Sf2;

		double sfAvg = (Sf[i] + Sf[i - 1]) * 0.5;
		hf_reach += sfAvg * std::abs(params.dx);

        // For PRESSURIZED flow, use the average friction slope to get energy, then derive pressure
        if (isFull)
        {
            H[i] = H[i - 1] + sfAvg * std::abs(params.dx);
            PoG[i] = H[i] - Z[i] - Y[i] - V[i] * V[i] / (2. * g);
        }
        // For NON-PRESSURIZED flow, use the individual elements of the energy equation (PoG = 0), this way the local energy loss (HL) in a hydraulic jump will be properly represented
        else
        {
            PoG[i] = 0;
            H[i] = Z[i] + Y[i] + PoG[i] + V[i] * V[i] / (2. * g);
        }

        // Final checks, in case empty or pressurized conditions reversed at this section
        // If conduit is NOT EMPTY any more in this section, recalculate (when total energy decreases as we go upstream, which is not possible)
        if (isEmpty && (H[i] - H[i - 1]) < 0)
        {
            isEmpty = false;
            e2[i] = true;
            H[i] = H[i - 1];
            y2k = H[i] - Z[i] - PoG[i] - V[i] * V[i] / (2. * g);
            i--;
        }
        // If flow gets UNPRESSURIZED in this section, recalculate
        else if (isFull && PoG[i] <= 0)
        {
            double yTemp = H[i] - Z[i] - V[i] * V[i] / (2. * g);
            if (yTemp < 0.98 * maxDepth)
            {
                isFull = false;
                p2[i] = true;
                PoG[i] = 0;
                y2k = H[i] - Z[i] - PoG[i] - V[i] * V[i] / (2. * g);
                i--;
            }
        }

        if (!isEmpty)
        {
            //// Save the results of this step for the next step.
            //params.E1 = params.E2;
            //params.Sf1 = params.Sf2;
        }

        lastArea = x2.A;
        volumeArea += x2.A;
    }

    // Compute the volume (the sum of the areas of the cross section
    // at each step minus the average of the first and last cross
    // section areas times the x-step).
    volume = (volumeArea - (lastArea + params.first_area) / 2.0) * fabs(params.dx);

    // If we terminated early, then return an error.
    if (i < nC)
    {
        return 1;
    }

    // If there was an error, then return it.
    else if (error != 0)
    {
        return 1;
    }

    // Otherwise we successfully solved for a point.
    else
    {
        yUp = y2 + PoG[nC];
        return 0;
    }
}


// This function computes the variables (theta, wetted perimeter, etc.)
// for later on.
inline void compute_variables(double y, solver_params& params, profile_params& x)
{
    x.A = params.xs->computeArea(y);
    if (isZero(x.A)) // is it zero?
    {
        profile_error_code = ERR_ZERO_AREA;
        return;
    }

    x.P = params.xs->computeWettedPerimiter(y);
    x.T = params.xs->computeTopWidth(y);
    x.V = params.Q / x.A;
    x.Y = y;
    //x.E = x.Z + y + params.Q * params.Q / (x.A * x.A * 2.0 * params.g);
    x.Sf = (params.Q * params.N * std::pow(x.P, 2.0/3.0)) / (params.kn * std::pow(x.A, 5.0/3.0));
    x.Sf = x.Sf * x.Sf; // square it
    x.PoG = 0;

    if (isZero(params.first_area + 1.0))
        params.first_area = x.A;
}


// This the function we're attempting to find the root for.
// This works for both steep, mild, and adverse.
inline double profile_func(double y, solver_params& params, profile_params& x1, profile_params& x2)
{
    compute_variables(y, params, x2);

    double E2 = (x2.Z + x2.Y + x2.V*x2.V/(2.*params.g) + x2.PoG);
    double E1 = (x1.Z + x1.Y + x1.V*x1.V/(2.*params.g) + x1.PoG);
    double fn = E2 - E1 - (x2.Sf + x1.Sf) / 2.0 * fabs(params.dx);
    //(x2.E - x1.E) * params.isSteep - (x2.Sf + x1.Sf) / 2.0 * fabs(params.dx);

    return fn;
}


// This is the derivative of the function we're attempting to find the
// root for.  This works for both steep, mild, and adverse.
inline double profile_func_deriv(double y, solver_params& params, profile_params& x)
{
    double Q = params.Q;
    double g = params.g;
    double A = x.A;
    double dx = fabs(params.dx);
    double n = params.N;
    double P = x.P;
    double T = x.T;

    double dPdy = params.xs->computeDpDy(y);
    double dAdy = params.xs->computeDaDy(y);

    double dfn = 1 - Q*Q * dAdy / (g * A*A*A) - Q*Q * n*n / 2. * dx *
        (
            4./3. * std::pow(P, ONETHIRD) / std::pow(A, TENTHIRDS) * dPdy - TENTHIRDS * dAdy * std::pow(P, FOURTHIRDS) / std::pow(A, THIRTEENTHIRDS)
        );

    return dfn * params.isSteep;
}

