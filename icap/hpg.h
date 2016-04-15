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

#ifndef __HPG_LIST_H________________________20080814110707__
#define __HPG_LIST_H________________________20080814110707__

#include <vector>
#include <string>
#include <memory>

#include "../hpg_interp/hpg.hpp"
#include "../util/parseable.h"
#include "../geometry/link_list.h"


#define HPG_ERROR -100
#define HPG_PIPE_FULL 1
#define HPG_PIPE_OK 0
#define HPG_PIPE_EMPTY -1


/// This class loads and keeps track of HPGs.
class IcapHpg : public Parseable
{
protected:
    std::map<id_type, std::shared_ptr<hpg::Hpg>> m_list;
    
    int m_hpgCount;

    //NormCritParams m_ncParams;
    //bool m_ncParamsInit;

	/// Loads the HPG, if checks pass.
    bool checkAndLoadHPG(std::shared_ptr<geometry::Link> link, const std::string& dirPath);

	/// Does the actual HPG loading.
    bool loadHPG(id_type linkId, const std::string& path);

    int m_currentHPG;

public:
    ~IcapHpg();
    IcapHpg();

	/// This pre-allocates the storage for the HPGs.
    bool allocate(int count);

	/// Returns the HPG for the given link index.
    std::shared_ptr<hpg::Hpg> getHpg(id_type idx);

	/// Returns the upstream interpolated from the Q and downstream.
    bool getUpstream(id_type linkId, var_type dsHead, var_type flow, var_type& usHead);

	/// Returns the H_f for the given Q/downstream.
    bool getHf(id_type linkId, var_type dsHead, var_type flow, var_type& hf);

	/// Returns the volume in the system for the backwater curve with the downstream head and Q.
	bool getVolume(id_type linkId, var_type dsHead, var_type flow, var_type& volume);

    //var_type getLowestFlow(int linkId, bool isAdverse);
    
	/// Loads all of the HPGs.
    bool loadHpgs(const std::string& path, geometry::LinkList* linkList);

	/// Loads one HPG and gets ready to load the next.
    int loadNextHpg(const std::string& path, geometry::LinkList* linkList);

    //bool IsValidFlow(int linkId, double flow);
    //bool CanInterpolate(int linkId, double dsDepth, double flow);
    //// 0 = ok, -1 = too small flow, +1 = too large flow
    //int CanInterpolateExtended(int linkId, double dsDepth, double flow);
    //bool GetCritUpstream(int linkId, double flow, double& usDepth);
    //bool PressurizedInterpolation(int linkId, double dsDepth, double flow, double& usDepth);
    //bool CalculateCriticalDepth(double flow, double diameter, double initGuess, double& hCrit);
    //int GetHPGError(int linkId);
};


#endif//__HPG_LIST_H________________________20080814110707__
