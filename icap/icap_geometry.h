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

#ifndef ICAP_GEOMETRY_H__
#define ICAP_GEOMETRY_H__

#include <string>

#include "../api.h"
#include "../geometry/geometry.h"
#include "../model/variables.h"

#include "rt_inflow.h"


class IcapGeometry : public geometry::Geometry, public Model
{
private:
    std::map<std::string, std::shared_ptr<RealTimeInflow>> rtInflowMap; // DO NOT delete in dtor

    DateTime currentDateTime;
    std::string hpgPath;
    double routeStep;
    double reportStep;
    bool freeSurfaceOnlyComputations;

protected:
    virtual bool processOptions();

public:

    std::string getHpgPath() { return this->hpgPath; }
    double getRoutingStep() { return this->routeStep; }
    double getReportStep() { return this->reportStep; }
    bool freeSurfaceOnly() { return this->freeSurfaceOnlyComputations; }
    void enableRealTimeStatus();

    ///////////////////////////////////////////////////////////////////////
    // Model interface method.
    virtual void resetTimestep();
    virtual void startTimestep(const DateTime& dateTime);
    virtual DateTime getCurrentDateTime();
    virtual void resetDepths();
    virtual void setRealTimeInputFlow(std::string nodeId, var_type flow);
    virtual void setRealTimeInputHead(std::string nodeId, var_type head);
    virtual void addRealTimeInput(std::string nodeId);
    virtual var_type getRealTimeNodeHead(std::string nodeId);
    virtual var_type getRealTimeNodeInflow(std::string nodeId);

    ///////////////////////////////////////////////////////////////////////
    // Model interface node/link variable access methods.
    virtual var_type getNodeVariable(id_type nodeId, variables::Variables var);
    virtual void setNodeVariable(id_type nodeId, variables::Variables var, var_type value);
    virtual var_type getLinkVariable(id_type linkId, variables::Variables var);
    virtual void setLinkVariable(id_type linkId, variables::Variables var, var_type value);

    ///////////////////////////////////////////////////////////////////////
    // Model interface statistics methods.
    /// <summary>
    /// This function updates the given node statistic by adding the given value.
    /// </summary>
    virtual void updateNodeStatistic(id_type nodeId, statvariables::StatVariables var, var_type value);
    /// <summary>
    /// This function updates the given system statistic by adding the given value.
    /// </summary>
    virtual void updateSystemStatistic(statvariables::StatVariables var, var_type value);
};


#endif//ICAP_GEOMETRY_H__

