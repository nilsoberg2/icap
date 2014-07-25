#ifndef NORMCRIT_H_HPG_
#define NORMCRIT_H_HPG_


#include "../xslib/reach.h"

/**
* Calculate the normal depth for given flow.
* @param flow double flow to calculate for
* @param normal_guess double initial guess (should be previous initial guess)
* @return double normal depth
*/
int ComputeNormalDepth(xs::Reach reach, double flow, double g, double ks, double& yN);

/**
* Calculate the critical depth for given flow
* @param flow double flow to calculate for
* @param normal_guess double initial guess (should be previous initial guess)
* @return double critical depth
*/
int ComputeCriticalDepth(xs::Reach reach, double flow, double g, double& yC);


#endif//NORMCRIT_H_HPG_
