#ifndef __ICAP_MATH_H_______________________20090902104343__
#define __ICAP_MATH_H_______________________20090902104343__


double ComputeCrossSectionArea(double diameter, double depth);
double ComputePipeVolume(double diameter, double slope, double length, double y_up);
double ComputeCrossSectionDepth(double diameter, double area);
double ComputePipeDSDepth(double diameter, double slope, double length, double volume);


#endif//__ICAP_MATH_H_______________________20090902104343__
