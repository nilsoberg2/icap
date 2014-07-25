
#include <string>
#include <boost/algorithm/string.hpp>

#include "cross_section.h"

#include "circular.h"
#include "dummy.h"


namespace xs
{
    std::shared_ptr<CrossSection> Factory::create(xstype xsType)
    {
        if (xsType == xstype::circular)
        {
            return std::shared_ptr<CrossSection>(new Circular());
        }
        else
        {
            return std::shared_ptr<CrossSection>(new Dummy());
        }
    }

    template<typename XSType>
    std::shared_ptr<CrossSection> Factory::create()
    {
        if (typeid(Circular) == typeid(XSType))
        {
            return std::shared_ptr<CrossSection>(new Circular());
        }
        else
        {
            return std::shared_ptr<CrossSection>(new Dummy());
        }
    }

    std::shared_ptr<CrossSection> Factory::create(const std::string& type)
    {
        std::string geom = boost::algorithm::to_lower_copy(type);
        
        if (geom == "circular")
        {
            return std::shared_ptr<CrossSection>(new Circular());
        }
        else// if (geom == "irregular")
        {
            return std::shared_ptr<CrossSection>(new Dummy());
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
