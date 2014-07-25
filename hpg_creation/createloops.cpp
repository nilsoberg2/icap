
#include <iostream>
#include <stdlib.h>
#include <algorithm>

#include "../hpg_interp/hpg.hpp"
#include "../hpg/math.hpp"
#include "../hpg/units.hpp"
#include "../hpg/exception.hpp"
//#include "../hpg/error.hpp"

#include "hpg_creator.hpp"

using namespace std;


std::shared_ptr<hpg::Hpg> HpgCreator::AutoCreateHPG(const xs::Reach& reach)
{
    // Create an HPG object and fill in the channel geometry.
    std::shared_ptr<hpg::Hpg> hpg = std::shared_ptr<hpg::Hpg>(new hpg::Hpg());
    hpg->SetDSInvert(reach.getDsInvert());
    hpg->SetUSInvert(reach.getUsInvert());

    if (reach.getDsStation() != hpg::error::bad_value)
    {
        hpg->SetDSStation(reach.getDsStation());
        hpg->SetUSStation(reach.getDsStation() + reach.getLength());
    }

    hpg->SetLength(reach.getLength());
    hpg->SetRoughness(reach.getRoughness());
    hpg->SetMaxDepth(reach.getMaxDepth());

    double pressurizedHeight = 1000;

    int numStepsSave = this->numSteps;
    this->numSteps = std::max(this->numSteps, (int)round(reach.getLength() / 10.0));

    // This loop is to allow use of the same code for computing both positive
    // and negative slopes.
    for (int slopeRev = 0; slopeRev < 2; slopeRev++)
    {
        // If we're on the second iteration of the loop, then reverse the flow.

        // Compute the maximum and minimum flows for this reach.  This is done using
        // a bisection method in combination with the ComputeValidHPGCurve method.
        double maxDepth = 0.0, minDepth = 0.0;
        double maxFlow = FindMaxFlow(reach, slopeRev, maxDepth);
        if (this->errorCode)
            break;
        double minFlow = 0;//FindMinFlow(reach, reach.getMaxDepth()*0.005, maxFlow, minDepth);
        if (this->errorCode)
            break;

        for (int isPress = 0; isPress < 2; isPress++)
        {
            if (isPress)
            {
                minFlow = maxFlow + (maxFlow - minFlow) / 100;
                auto x = reach.getXs();
                maxFlow = x->computeArea(x->getMaxDepth()) * 2. * std::sqrt(2. * this->g * pressurizedHeight - reach.getDsInvert());
            }

            // Determine the flow spacing.
            deque<double> flows;
            FindFlowIncrements(reach, slopeRev, minFlow, maxFlow, flows);
            if (flows.size() == 0)
                continue;
            //if (flows.back() < maxFlow && abs(flows.back() - maxFlow) > 10.0)
            //    flows.push_back(round(maxFlow, 1.0));
            //else if (flows.back() > maxFlow)
            //    flows.back() = round(maxFlow, 1.0);

            double yNormal = 0.0;
            double yCritical = 0.0;

            // For every flow, compute a curve.
            for (unsigned int i = 0; i < flows.size(); i++)
            {
                double curFlow = flows.at(i);

                // Calculate a backwater profile.  ComputeHPGCurve() automatically
                // determines slope type (from normal/critical) and calculates
                // accordingly.
                hpg::hpgvec curve;
                bool valid = ComputeValidHPGCurve(reach, curFlow, pressurizedHeight, slopeRev, yNormal, yCritical, curve);

                // Check if an error occurred.
                if (valid)
                {
                    // If the curve has more than this->minCurvePoints add it to the HPG.
                    if ((int)curve.size() >= this->minCurvePoints)
                    {
                        if (slopeRev)
                            hpg->AddCurve(-curFlow, curve, (hpg::point)(curve.at(0)));
                        else
                            hpg->AddCurve(curFlow, curve, (hpg::point)(curve.at(0)));
                    }
                }
                else
                    // There was an error, so don't continue.
                    break;
            }

            // Clear the error code so that any valid HPCs get saved.
            this->errorCode = 0;
        }
    }

    this->numSteps = numStepsSave;

    if (this->errorCode)
    {
        return NULL;
    }
    else
    {
        return hpg;
    }
}

