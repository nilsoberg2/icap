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


#include <string>
#include <boost/algorithm/string.hpp>

#include "cross_section.h"

#include "circular.h"
#include "dummy.h"


namespace xs
{
    CrossSection* Factory::create(const CrossSection* xs)
    {
        if (xs->getType() == xstype::circular)
        {
            return new Circular((Circular*)xs);
        }
        else
        {
            return new Dummy();
        }
    }

    //std::shared_ptr<CrossSection>
    CrossSection* Factory::create(xstype xsType)
    {
        if (xsType == xstype::circular)
        {
            return new Circular();
        }
        else
        {
            return new Dummy();
        }
    }

    template<typename XSType>
    //std::shared_ptr<CrossSection>
    CrossSection* Factory::create()
    {
        if (typeid(Circular) == typeid(XSType))
        {
            return new Circular();
        }
        else
        {
            return new Dummy();
        }
    }

    //std::shared_ptr<CrossSection> Factory::create(const std::string& type)
    CrossSection* Factory::create(const std::string& type)
    {
        std::string geom = boost::algorithm::to_lower_copy(type);
        
        if (geom == "circular")
        {
            return new Circular();
        }
        else// if (geom == "irregular")
        {
            return new Dummy();
        }
    }
}

//
//double ComputeCrossSectionDepth(double diameter, double area)
//{
//#define A area
//#define D diameter
//
//	double depth = 0.0;
//	double C = 8.0 * A / (D * D);
//    
//    //f(x)  = C - x + sin(x)
//    //f'(x) = -x + cos(x)
//    
//    double x1 = M_PI;
//    double x2 = 0;
//    
//    double converg = 1e-4;
//    int maxIter = 10;
//    
//    double f = 0.0;
//	double df = 0.0;
//    int i = 0;
//    while (i++ < maxIter && fabs(f) > converg)
//	{
//        f = (C - x1 + sin(x1));
//        df = (-1 + cos(x1));
//        x2 = x1 - f / df;
//        x1 = x2;
//	}
//
//	if (fabs(f) > converg)
//		return -1.0;
//    
//    return D / 2 * (1 - cos(x1/2));
//
//#undef A
//#undef D
//}
