#ifndef NORMCRIT_H_HPG_
#define NORMCRIT_H_HPG_


#include "../xslib/reach.h"

/**
* Calculate the normal depth for given flow.
* @param flow double flow to calculate for
* @param double normal depth
* @return non-zero if error
*/
int ComputeNormalDepth(const xs::Reach& reach, double flow, double g, double kn, double& yN);

/**
* Calculate the normal flow for given depth.
* @param depth double depth to calculate for
* @return double normal depth
*/
int ComputeNormalFlow(const xs::Reach& reach, double depth, double g, double kn, double& qNormal);

/**
* Calculate the critical depth for given flow
* @param flow double flow to calculate for
* @param normal_guess double initial guess (should be previous initial guess)
* @return non-zero if error
*/
int ComputeCriticalDepth(const xs::Reach& reach, double flow, double g, double& yC);


int ComputeCriticalFlow(const xs::Reach& reach, double depth, double g, double& qCritical);


#endif//NORMCRIT_H_HPG_
