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

