
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
