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


#include <algorithm>

#include "hpg_creator.hpp"


HpgCreator::HpgCreator()
{

    this->convergenceTol = 0.000001;
    this->numHpc = 20;
    this->numPoints = 40;
    this->maxDepthFrac = 1.;
    this->maxIterations = 40;
    this->maxIterations = 100;
    this->minCurvePoints = 4;
    this->errorCode = 0;
    this->numSteps = 1000.;

    setUnits(HpgUnits::Hpg_English);
}


double HpgCreator::getConvergenceTolerance()
{
    return this->convergenceTol;
}


void HpgCreator::setConvergenceTolerance(double factor)
{
    this->convergenceTol = std::min(1e-4, factor);
}


int HpgCreator::getNumberOfCurves()
{
    return this->numHpc;
}


void HpgCreator::setNumberOfCurves(int curves)
{
    this->numHpc = curves;
}


int HpgCreator::getNumberOfPointsPerCurve()
{
    return this->numPoints;
}


void HpgCreator::setNumberOfPointsPerCurve(int points)
{
    this->numPoints = std::max(points, 10);
}


double HpgCreator::getMaxDepthFraction()
{
    return this->maxDepthFrac;
}


void HpgCreator::setMaxDepthFraction(double depth)
{
    this->maxDepthFrac = std::max(0., std::min(1., depth));
}


int HpgCreator::getMaxIterations()
{
    return this->maxIterations;
}


void HpgCreator::setMaxIterations(int iterations)
{
    this->maxIterations = std::max(20, iterations);
}


void HpgCreator::setUnits(int u)
{
    if (u == HpgUnits::Hpg_English)
    {
        this->g = 32.174;
        this->kn = 1.486;
    }
    else if (u == HpgUnits::Hpg_SI)
    {
        this->g = 9.81;
        this->kn = 1.0;
    }
}


int HpgCreator::getMinCurveSize()
{
    return this->minCurvePoints;
}


void HpgCreator::setMinCurveSize(int size)
{
    this->minCurvePoints = std::max(4, size);
}


int HpgCreator::getError()
{
    return this->errorCode;
}


int HpgCreator::getNumBackwaterSteps()
{
    return this->numSteps;
}


void HpgCreator::setNumBackwaterSteps(int numComp)
{
    this->numSteps = std::max(20, numComp);
}
