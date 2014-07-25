
#define _USE_MATH_DEFINES
#include <cmath>

#include "../hpg/error.hpp"

#include "hpg_creator.hpp"
#include "benchmark.h"
#include "normcrit.h"
#include "profile.h"


// NOTE: BENCH_* macros will be empty unless BENCHMARKYES is defined
// (usually in project settings)

void HpgCreator::ComputeHPGCurve(const xs::Reach& reach, double flow, double pressurizedHeight, bool reverseSlope, double& yNormal, double& yCritical, hpg::hpgvec& curve)
{
    BENCH_INIT;

    double slope = reach.getSlope() * (reverseSlope ? -1. : 1.);
    double maxDepth = reach.getMaxDepth();
    double dsInvert = reach.getDsInvert();
    double usInvert = reach.getUsInvert();

    // Normal depth is only defined for positive slope channels.
    if (slope >= 0.0)
    {
        BENCH_START;
    	
        if (ComputeNormalDepth(reach, flow, this->g, this->kn, yNormal))
        {
            this->errorCode = hpg::error::divergence;
            return;
        }
        
        BENCH_STOP;
    }

    BENCH_START;

	if (ComputeCriticalDepth(reach, flow, this->g, yCritical))
    {
        this->errorCode = hpg::error::divergence;
        return;
    }
        
    BENCH_STOP;


    // isSteep = true if the channel is steep-slope and the slope is positive.
    bool isSteep = (yCritical > yNormal && slope >= 0.0);

    // Set the min/max according to the channel characteristics (steep/mild).
    double yMin, yMax;
    if (isSteep)
    {
        // The following two lines would be the case if this was an S2 curve (supercritical flow).
        //yMin = diameter * 0.005; // 0.5% of diameter is the minimum
        //yMax = yCritical;
        // This is the case for S1 curves.
        yMin = yCritical;
        yMax = this->maxDepthFrac * maxDepth;
    }
    else
    {
        yMin = yCritical;
        yMax = this->maxDepthFrac * maxDepth;
    }

    bool computeFreeOnly = pressurizedHeight < 1e-6;

    // Compute the depth increment in vertical direction.
    double dy = (yMax - yMin) / this->numPoints;

    // If the user specified a pressurized height, then we do two executions of the loop, the first
    // to do the free-surface computations and the second to do the pressurized computations.
    // If the pressurized height is zero, then just do the free-surface computations.
    int freePressLoopActivation = pressurizedHeight > 0 ? 2 : 1;
    for (int i = 0; i < freePressLoopActivation; i++)
    {
        // Starting depth.
        double yInit = yMin;
        if (i > 0)
        {
            yInit = yMax + dy;
            yMax += pressurizedHeight;
            dy = (yMax - yMin) / this->numPoints;
        }

        // We iterate until we've reached or exceeded the maximum depth.
        int count = 0;
        while (std::abs(yInit - yMax) > this->convergenceTol && yInit <= yMax)
        {
            this->errorCode = 0;

            double volume = 0.0;
            double yComp = 0.0;
		    double hf_reach = 0;

            // Now compute the point (downstream -> upstream if mild or
            // adverse, upstream -> downstream if steep).
            if (computeFreeOnly)
            {
                this->errorCode = ComputeFreeProfile(reach, flow, yInit, this->numSteps, isSteep, reverseSlope, this->g, this->kn, this->maxDepthFrac, yComp, volume, hf_reach);
            }
            else
            {
                this->errorCode = ComputeCombinedProfile(reach, flow, yInit, this->numSteps, isSteep, reverseSlope, this->g, this->kn, this->maxDepthFrac, yComp, volume, hf_reach);
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
                if (isSteep)
                {
                    curve.push_back(hpg::point(yComp + dsInvert,
                                                 yInit + usInvert,
                                                 volume,
											     hf_reach));
                }
			    //else if (slope < 0.0)
                //{
			    //	curve.push_back(hpg::point(yComp + dsInvert,
			    //								 //yInit + dsInvert+(length*slope),
                //                                 //volume));
                //}

			    else if (slope < 0.0)
                {
				    curve.push_back(hpg::point(yInit + usInvert,
											     yComp + dsInvert,
                                                 volume,
											     hf_reach));
                }

                else
                {
                    curve.push_back(hpg::point(yInit + dsInvert,
                                                 yComp + usInvert,
                                                 volume,
											     hf_reach));
                }

                count++;
            }

            // Go to the next depth.
            yInit = yInit + dy;
        }
    }
}


// This is a wrapper around ComputeHPGCurve.  It returns true if a valid
// HPC was computed and false otherwise.  If false, then it also clears
// the curve variable so that no possibly bad values are stored.
bool HpgCreator::ComputeValidHPGCurve(const xs::Reach& reach, double flow, double pressurizedHeight, bool reverseSlope, double& yNormal, double& yCritical, hpg::hpgvec &curve)
{
    this->errorCode = 0;

    ComputeHPGCurve(reach, flow, pressurizedHeight, reverseSlope, yNormal, yCritical, curve);

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
