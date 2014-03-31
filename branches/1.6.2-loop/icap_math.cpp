#define _USE_MATH_DEFINES
#include "icap_math.h"
#include <cmath>


double ComputeCrossSectionArea(double diameter, double depth)
{
#define y depth
#define D diameter
#define r (diameter / 2.0)

	if (y > D)
		return M_PI * r * r;
	if (y <= 0.0)
		return 0.0;

	double theta = 2.0 * acos(1.0 - 2.0 * y / D);
	return D * D / 8.0 * (theta - sin(theta));

#undef y
#undef D
#undef r
}


double ComputeCrossSectionDepth(double diameter, double area)
{
#define A area
#define D diameter

	double depth = 0.0;
	double C = 8.0 * A / (D * D);
    
    //f(x)  = C - x + sin(x)
    //f'(x) = -x + cos(x)
    
    double x1 = M_PI;
    double x2 = 0;
    
    double converg = 1e-4;
    int maxIter = 10;
    
    double f = 0.0;
	double df = 0.0;
    int i = 0;
    while (i++ < maxIter && fabs(f) > converg)
	{
        f = (C - x1 + sin(x1));
        df = (-1 + cos(x1));
        x2 = x1 - f / df;
        x1 = x2;
	}

	if (fabs(f) > converg)
		return -1.0;
    
    return D / 2 * (1 - cos(x1/2));

#undef A
#undef D
}


double ComputePipeVolume(double diameter, double slope, double length, double y_up)
{
#define y2 y_up
#define L length
#define S slope
#define D diameter

	double dx = (L / 100.0);
	double xi = 0.0;
	double V = 0.0;
	double yi, a1, a2;

	while (xi < L)
	{
		yi = (-S * (xi - L) + y2);
		a1 = ComputeCrossSectionArea(D, yi);
		
		xi += dx;
		yi = (-S * (xi - L) + y2);
		a2 = ComputeCrossSectionArea(D, yi);

		V += dx * (a1 + a2) / 2.0;
	}

	return V;

#undef y2
#undef L
#undef S
#undef D
}
