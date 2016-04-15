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

#if defined(_DEBUG)
#include <Windows.h>
#endif

#include <deque>

#include "../hpg/error.hpp"
#include "../util/math.h"

#include "hpg_creator.hpp"


// Compute the maximum flow for the reach with the given characteristics.
// This is done using a combination of a bisection method and computeValidHpgCurve.
// A valid flow is one that has more than this->minCurvePoints points.
double HpgCreator::findMaxFlow(const xs::Reach& reach, bool reverseSlope, double &yCritMax)
{
    double yNormal, yCritical;
    this->errorCode = 0;

    // First determine the upper bound; a high flow that doesn't have a valid HPC.
    double end = reach.getMaxDepth() * 500;

    hpg::hpgvec curve;
    bool valid = computeValidHpgCurve(reach, end, 0., reverseSlope, yNormal, yCritical, curve);
    int iterations = 0;
    while (valid && iterations < 50)
    {
        end += 100 * reach.getMaxDepth();
        curve.clear();
        valid = computeValidHpgCurve(reach, end, 0., reverseSlope, yNormal, yCritical, curve);
        iterations++;
    }

    // If there was no convergence, then return an error.
    if (iterations == 50)
    {
        end = hpg::error::bad_value;
        this->errorCode = hpg::error::divergence;
        return end;
    }

    // Now determine the maximum flow which should be between 1 and end.
    double start = 1;
    
    curve.clear();
    iterations = 0;
    while (fabs(end - start) > 1 && iterations < 50)
    {
        double guess = 0.5 * (end - start) + start;
        valid = computeValidHpgCurve(reach, guess, 0., reverseSlope, yNormal, yCritical, curve);
        if (valid)
        {
            yCritMax = yCritical;
            start = guess;
        }
        else
            end = guess;
        curve.clear();
        iterations++;
    }

    // If there was no convergence, then return an error.
    if (iterations == 50)
    {
        end = hpg::error::bad_value;
        this->errorCode = hpg::error::divergence;
    }
    else
        this->errorCode = 0;

    return end;
}


//// Compute the minimum flow for the reach with the given characteristics.
//// This is done using a combination of a bisection method and computeValidHpgCurve.
//// A valid flow is one that has more than this->minCurvePoints points.
//double HpgCreator::FindMinFlow(double start, double end, double diameter, double length, double roughness, double slope, double dsInvert, double &y_critical_min)
//{
//    double yNormal, yCritical;
//    this->errorCode = 0;
//
//    // The upper bound has been already determined (end).  Now determine the lowest valid flow.
//
//    bool valid = false;
//    hpg::hpgvec curve;
//    int iterations = 0;
//    while (fabs(end - start) > 0.01 && iterations < 50)
//    {
//        double guess = 0.5 * (end - start) + start;
//        valid = computeValidHpgCurve(guess, diameter, length, roughness, slope, dsInvert, yNormal, yCritical, curve);
//        if (valid)
//        {
//            end = guess;
//            y_critical_min = yCritical;
//        }
//        else
//            start = guess;
//        curve.clear();
//        iterations++;
//    }
//
//    if (iterations == 50)
//    {
//        end = hpg::error::bad_value;
//        this->errorCode = hpg::error::divergence;
//    }
//    else
//        this->errorCode = 0;
//  
//    // set the flow to be slightly higher than the minimum
//    //NOTE: this should probably be scaled to the range
//    end += 0.1;
//
//    return end;
//}


// Determine the spacing between curves.
void HpgCreator::findFlowIncrements(const xs::Reach& reach, bool reverseSlope, double minDepth, double maxDepth, std::deque<double> &flows)
{
    this->errorCode = 0;

    //for (int i = 0; i < this->numHpc; i++)
    //{
    //    double fac = ((double)i) * (1./this->numHpc);
    //    double fac2 = 1. - 1. / this->numHpc;
    //    double flow = minFlow + std::log10(1. - fac*fac*fac) / std::log10(1. - fac2*fac2*fac2) * (maxFlow - minFlow);
    //    flows.push_back(flow);
    //}

    double effective_max_depth = maxDepth - minDepth;
    for (int i = 0; i <= this->numHpc; i++)
    {
        double depth = effective_max_depth * (double)i/(double)this->numHpc + minDepth;

        double criticalFlow;
        if (!isZero(depth))
        {
            auto x = reach.getXs();
            double A = x->computeArea(depth);
            double T = x->computeTopWidth(depth);
            criticalFlow = std::sqrt( A * A * A * this->g / T );
            flows.push_back(criticalFlow);
        }
        else if (!reverseSlope)
        {
            flows.push_back(0);
            criticalFlow = 0;
        }


        //OutputDebugStringA(std::to_string(criticalFlow).c_str());
        //OutputDebugStringA(" ");
        //OutputDebugStringA(std::to_string(depth).c_str());
        //OutputDebugStringA("\n");
    }
}

void HpgCreator::findFlowIncrementsByFlow(const xs::Reach& reach, bool reverseSlope, double minFlow, double maxFlow, std::deque<double> &flows)
{
    this->errorCode = 0;

    for (int i = 0; i < this->numHpc; i++)
    {
        double fac = ((double)i) * (1./this->numHpc);
        double fac2 = 1. - 1. / this->numHpc;
        double flow = minFlow + std::log10(1. - fac*fac*fac) / std::log10(1. - fac2*fac2*fac2) * (maxFlow - minFlow);
        flows.push_back(flow);
    }

    //double effective_max_depth = maxDepth - minDepth;
    //for (int i = 0; i <= this->numHpc; i++)
    //{
    //    double depth = effective_max_depth * (double)i/(double)this->numHpc + 0.1 + minDepth;

    //    auto x = reach.getXs();
    //    double A = x->computeArea(depth);
    //    double T = x->computeTopWidth(depth);
    //    double criticalFlow = std::sqrt( A * A * A * this->g / T );

    //    if (! this->errorCode)
    //        flows.push_back( criticalFlow );
    //}
}
