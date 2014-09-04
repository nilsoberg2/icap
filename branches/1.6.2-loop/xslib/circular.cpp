
#define _USE_MATH_DEFINES
#include <cmath>
#include <numeric>
#include <limits>

#include "../util/parse.h"
#include "../util/math.h"

#include "circular.h"


namespace xs
{
    void Circular::init()
    {
        CrossSection::xsType = xstype::circular;
    }

    Circular::Circular()
        : diameter(1)
    {
        init();
    }

    Circular::Circular(double diam)
        : diameter(diam)
    {
        init();
    }

    Circular::Circular(const Circular* rhs)
    {
        init();
        this->diameter = rhs->diameter;
        this->lastDepth = rhs->lastDepth;
        this->theta = rhs->theta;
    }

    std::shared_ptr<CrossSection> Circular::clone()
    {
        return std::shared_ptr<Circular>(new Circular(this));
    }

    double Circular::getTheta(double y)
    {
        if (isZero(y - lastDepth))
        {
            return theta;
        }

	    if (y >= diameter)
		    theta = 2 * M_PI;
	    else if (y <= 0.0)
		    theta = 0.0;
        else
    	    theta = 2.0 * acos(1.0 - 2.0 * y / diameter);

        return theta;
    }

    double Circular::computeArea(double y)
    {
        double theta = getTheta(y);
	    return diameter * diameter / 8.0 * (theta - sin(theta));
    }
    
    double Circular::computeWettedPerimiter(double y)
    {
        double theta = getTheta(y);
        return 0.5 * theta * diameter;
    }

    double Circular::computeTopWidth(double y)
    {
        double theta = getTheta(y);
        return diameter * sin(theta / 2.0);
    }

    double Circular::computeDpDy(double y)
    {
        return 1.0 / std::sqrt(y / diameter * (1. - y / diameter));
    }

    double Circular::computeDaDy(double y)
    {
        return computeTopWidth(y);
    }

    double Circular::computeDtDy(double y)
    {
        return (diameter - 2. * y) / std::sqrt(y * diameter - y * y);
    }

    bool Circular::setParameters(std::vector<std::string>::const_iterator firstPart, std::vector<std::string>::const_iterator end)
    {
        if (firstPart == end)
        {
            return false;
        }

        if (!tryParse(*firstPart, this->diameter))
        {
            setErrorMessage("Unable to parse max depth");
            return false;
        }

        return true;
    }
}

