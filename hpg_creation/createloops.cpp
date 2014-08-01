
#include <iostream>
#include <stdlib.h>
#include <algorithm>

#include "../hpg_interp/hpg.hpp"
#include "../hpg/math.hpp"
#include "../hpg/units.hpp"
#include "../hpg/exception.hpp"
#include "normcrit.h"
//#include "../hpg/error.hpp"

#include "hpg_creator.hpp"

using namespace std;


std::shared_ptr<hpg::Hpg> HpgCreator::AutoCreateHpg(const xs::Reach& reach)
{
    // Create an HPG object and fill in the channel geometry.
    std::shared_ptr<hpg::Hpg> hpg = std::shared_ptr<hpg::Hpg>(new hpg::Hpg());
    hpg->setDsInvert(reach.getDsInvert());
    hpg->setUsInvert(reach.getUsInvert());

    if (reach.getDsStation() > hpg::error::bad_value)
    {
        hpg->setDsStation(reach.getDsStation());
        hpg->setUsStation(reach.getDsStation() + reach.getLength());
    }
    else
    {
        hpg->setDsStation(0);
        hpg->setUsStation(reach.getLength());
    }

    hpg->setLength(reach.getLength());
    hpg->setRoughness(reach.getRoughness());
    hpg->setMaxDepth(reach.getMaxDepth());
    hpg->setMaxDepthFraction(this->maxDepthFrac);

    double pressurizedHeight = 1000;

    int numStepsSave = this->numSteps;
    this->numSteps = std::max(this->numSteps, (int)round(reach.getLength() / 10.0));

    // This loop is to allow use of the same code for computing both positive
    // and negative slopes.
    for (int slopeRev = 0; slopeRev < 2; slopeRev++)
    {
        // If we're on the second iteration of the loop, then reverse the flow.

        // Compute the maximum and minimum flows for this reach.  This is done using
        // a bisection method in combination with the computeValidHpgCurve method.
        double yCritMax;
        double maxFlow = findMaxFlow(reach, slopeRev, yCritMax);
        if (this->errorCode)
            break;
        //double maxFlow = 0;
        //if (ComputeNormalFlow(reach, reach.getMaxDepth() * 0.9, this->g, this->kn, maxFlow))
        //{
        //    break;
        //}

        // This loop is to allow the same code to be used for computing fully-pressurized curves.
        // The first iteration is for curves that are supposed to to for curves that are free-surface
        // or start out free surface.  For the second iteration, we simply change the min and max flow
        // variables to start out differently.
        for (int isPress = 0; isPress < 2; isPress++)
        {
            double maxDepth = yCritMax; //this->maxDepthFrac * reach.getMaxDepth();
            double minDepth = 0.0;

            // Determine the flow spacing.
            deque<double> flows;
            if (isPress)
            {
                double minFlow = maxFlow * 1.02; // 102%
                auto x = reach.getXs();
                double headDiff = (pressurizedHeight - reach.getDsInvert());
                maxFlow = x->computeArea(x->getMaxDepth()) * 2. * std::sqrt(2. * this->g * headDiff); // I pulled the *3 out of nowhere hahaha
                for (int i = 0; i < 20; i++)
                {
                    flows.push_back(minFlow + (maxFlow - minFlow) * (double)(i) / 20.);
                }
            }
            else
            {
                findFlowIncrements(reach, slopeRev, minDepth, maxDepth, flows);
            }

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

                // Calculate a backwater profile.  computeHpgCurve() automatically
                // determines slope type (from normal/critical) and calculates
                // accordingly.
                hpg::hpgvec curve;
                bool valid = computeValidHpgCurve(reach, curFlow, pressurizedHeight, slopeRev, yNormal, yCritical, curve);

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
                //else
                //    // There was an error, so don't continue.
                //    break;
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

