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

