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


#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <algorithm>

#include "../hpg/error.hpp"
#include "../util/math.h"

#include "hpg_creator.hpp"
#include "benchmark.h"
#include "normcrit.h"
#include "profile.h"


// NOTE: BENCH_* macros will be empty unless BENCHMARKYES is defined
// (usually in project settings)

void HpgCreator::computeHpgCurve(const xs::Reach& reach, double flow, double pressurizedHeight, bool reverseSlope, double& yNormal, double& yCritical, hpg::hpgvec& curve)
{
    BENCH_INIT;

    bool computeFreeOnly = pressurizedHeight < 1e-6;

    //double slope = reach.getSlope();// * (reverseSlope ? -1. : 1.);
    double maxDepth = reach.getMaxDepth();
    double dsInvert = reach.getDsInvert();
    //if (reverseSlope)
    //    dsInvert = reach.getUsInvert();
    double usInvert = reach.getUsInvert();
    //if (reverseSlope)
    //    usInvert = reach.getDsInvert();

    bool yNvalid = false;
    // Normal depth is only defined for positive slope channels.
    if (!reverseSlope && !isZero(flow))
    {
        BENCH_START;
    	
        if (ComputeNormalDepth(reach, flow, this->g, this->kn, yNormal))
        {
            if (computeFreeOnly)
            {
                this->errorCode = hpg::error::divergence;
                return;
            }
        }
        else
        {
            yNvalid = true;
        }
        
        BENCH_STOP;
    }

    BENCH_START;

    bool yCvalid = false;
	if (!isZero(flow))
    {
        if (!ComputeCriticalDepth(reach, flow, this->g, yCritical))
        {
            yCvalid = true;
        }
        else
        {
            //this->errorCode = hpg::error::divergence;
            //return;
        }
    }
        
    BENCH_STOP;


    // isSteep = true if the channel is steep-slope and the slope is positive.
    bool isSteep = (yNvalid && yCvalid && yCritical > yNormal && !reverseSlope);

    // Set the min/max according to the channel characteristics (steep/mild).
    double yMin, yMax, yDthatMakesSteepYNormal;
    double volume = 0.0;
    double yComp = 0.0;
    double hf_reach = 0;
    if (isSteep)
    {
        // This is the case for S1 curves.
        if (yNvalid)
        {
            yMin = yNormal;
            // Find the point at which the downstream starts to influence the upstream (e.g. there
            // is no change from S1 profile to S2 [HJ present]).
            double yInit = yNormal;
            double dy = 0.05; // small dy to find the point we are trying to find
            double yComp = yNormal;
            double yDlast = yNormal;
            // We iterate until we've reached or exceeded the maximum depth.
            int count = 0;
            this->errorCode = 0;
            while (std::abs(yComp - yNormal) <= dy)
            {
                yDlast = yInit;
                // Go to the next depth.
                yInit = yInit + dy;

                this->errorCode = ComputeCombinedProfile(reach, flow, yInit, this->numSteps, false, reverseSlope, this->g, this->kn, this->maxDepthFrac, yComp, volume, hf_reach);

                // If the solution went imaginary, did not converge, or reached
                // the maximum pipe depth and not enough points were found, then
                // terminate early.  If there was another error (at_min_depth)
                // then continue to the next higher depth.
                if (this->errorCode)
                {
                    if (this->errorCode == hpg::error::imaginary)
                        break;
                    else if (this->errorCode == hpg::error::divergence)
                        break;
                    else if (this->errorCode == hpg::error::at_max_depth && count < this->minCurvePoints)
                        break;
                }
            }

            if (this->errorCode == 0)
            {
                yMin = yInit;
                yDthatMakesSteepYNormal = yDlast;
                //yDthatMakesSteepYNormal = (yInit - yNormal) / 2.0;
                //this->errorCode = ComputeCombinedProfile(reach, flow, yDthatMakesSteepYNormal, this->numSteps, false, reverseSlope, this->g, this->kn, this->maxDepthFrac, yComp, volume, hf_reach);
            }
        }
        else
        {
            yMin = maxDepth * 0.01; // 1% of diameter
            yDthatMakesSteepYNormal = yMin;
        }
        yMax = this->maxDepthFrac * maxDepth;
    }
    else
    {
        if (yCvalid)
            yMin = yCritical;
        else
            yMin = maxDepth * 0.01; // 1% of diameter
        yMax = this->maxDepthFrac * maxDepth;
    }

    int np = this->numPoints;

    if (yMax < yMin)
        yMax = yMin;

    if (isZero(yMax - yMin))
    {
        np = 1;
    }
    else
    {
        np = std::min(this->numPoints, std::max(1, (int)std::floor((yMax - yMin) / (0.01 * maxDepth) + 0.5)));
    }

    std::vector<double> yDownElevations;
    double point10p = (yMax - yMin) * 0.1;
    double pointYmax = yMax - point10p;
    double dy = (pointYmax - yMin) / np;
    for (int i = 0; i < np; i++)
    {
        yDownElevations.push_back(yMin + i * dy);
    }
    dy = point10p / np;
    for (int i = 0; i < np; i++)
    {
        yDownElevations.push_back(pointYmax + i * dy);
    }
    //dy = (pressurizedHeight - yMax) / this->numPoints;
    if (!computeFreeOnly)
    {
        for (int i = 0; i < this->numPoints; i++)
        {
            double i_np = (double)i / (double)this->numPoints;
            double temp = 1.0 - 1.0 / (double)this->numPoints;
            double y = yMax + std::log10(1.0 - i_np * i_np * i_np) / std::log10(1 - temp * temp * temp) * (pressurizedHeight - yMax);
            yDownElevations.push_back(y);
        }
    }

    // Starting depth.
    double yInit = yMin;

    // Add a point here that makes yNormal
    if (isSteep)
    {
        curve.push_back(hpg::point(yDthatMakesSteepYNormal + dsInvert, yNormal + usInvert, volume, hf_reach));
    }

    // We iterate until we've reached or exceeded the maximum depth.
    int count = 0;
    for (auto yInit : yDownElevations)
    {
        this->errorCode = 0;

        // Now compute the point (downstream -> upstream if mild or
        // adverse, upstream -> downstream if steep).
        if (computeFreeOnly)
        {
            this->errorCode = ComputeFreeProfile(reach, flow, yInit, this->numSteps, false, reverseSlope, this->g, this->kn, this->maxDepthFrac, yComp, volume, hf_reach);
        }
        else
        {
            this->errorCode = ComputeCombinedProfile(reach, flow, yInit, this->numSteps, false, reverseSlope, this->g, this->kn, this->maxDepthFrac, yComp, volume, hf_reach);
        }

        // If the solution went imaginary, did not converge, or reached
        // the maximum pipe depth and not enough points were found, then
        // terminate early.  If there was another error (at_min_depth)
        // then continue to the next higher depth.
        if (this->errorCode)
        {
            if (this->errorCode == hpg::error::imaginary)
                break;
            else if (this->errorCode == hpg::error::divergence)
                break;
            else if (this->errorCode == hpg::error::at_max_depth && count < this->minCurvePoints)
                break;
        }

        // If there was no error, then add the point to the curve.
        if (! this->errorCode)
        {
            if (reverseSlope)
            {
				curve.push_back(hpg::point(yInit + usInvert, yComp + dsInvert, volume, hf_reach));
            }

            else
            {
                curve.push_back(hpg::point(yInit + dsInvert, yComp + usInvert, volume, hf_reach));
            }

            count++;
        }
    }
}


#if defined(_DEBUG)
#include <Windows.h>
#endif


// This is a wrapper around computeHpgCurve.  It returns true if a valid
// HPC was computed and false otherwise.  If false, then it also clears
// the curve variable so that no possibly bad values are stored.
bool HpgCreator::computeValidHpgCurve(const xs::Reach& reach, double flow, double pressurizedHeight, bool reverseSlope, double& yNormal, double& yCritical, hpg::hpgvec &curve)
{
    this->errorCode = 0;
    using namespace std;

    computeHpgCurve(reach, flow, pressurizedHeight, reverseSlope, yNormal, yCritical, curve);

    // We want to clear the error code if there was any, but we got valid points.
    if ((!this->errorCode ||
         this->errorCode == hpg::error::divergence ||
         this->errorCode == hpg::error::at_max_depth ||
         this->errorCode == hpg::error::imaginary ||
         this->errorCode == hpg::error::at_min_depth) &&
         curve.size() > this->minCurvePoints)
    {
        this->errorCode = 0;
        return true;
    }
    else
    {
        curve.clear();
        return false;
    }
}
