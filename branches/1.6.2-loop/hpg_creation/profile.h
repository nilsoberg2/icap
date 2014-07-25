#ifndef PROFILE_HPG_H__
#define PROFILE_HPG_H__


#include "../api.h"
#include "../xslib/reach.h"


int ComputeFreeProfile(const xs::Reach& reach, double flow, double yInit, int nC, bool isSteep, bool reverseSlope, double g, double kn, double maxDepthFrac, double& yUp, double& volume, double& hf_reach);
int ComputeCombinedProfile(const xs::Reach& reach, double flow, double yInit, int nC, bool isSteep, bool reverseSlope, double g, double kn, double maxDepthFrac, double& yUp, double& volume, double& hf_reach);


#endif//PROFILE_HPG_H__
