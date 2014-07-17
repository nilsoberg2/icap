
#define _USE_MATH_DEFINES
#include <cmath>

#include "../util/parse.h"

#include "circular.h"


namespace xs
{
    Circular::Circular()
        : diameter(1)
    {
        CrossSection::xsType = xstype::circular;
    }

    Circular::Circular(double diam)
        : diameter(diam)
    {
        CrossSection::xsType = xstype::circular;
    }

    double Circular::computeAreaForDepth(double y)
    {
        double D = diameter;

	    if (y > D)
		    return M_PI * D * D / 4.0;
	    if (y <= 0.0)
		    return 0.0;

	    double theta = 2.0 * acos(1.0 - 2.0 * y / D);
	    return D * D / 8.0 * (theta - sin(theta));
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

