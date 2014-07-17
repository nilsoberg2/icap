#ifndef MODEL_H__
#define MODEL_H__

#include <string>

#include "../time/datetime.h"
#include "../api.h"

#include "variables.h"


typedef int id_type;
#define INVALID_IDX         -1


class Model
{
public:
    virtual void resetTimestep() = 0;
    virtual void startTimestep(DateTime dateTime) = 0;
    virtual DateTime getCurrentDateTime() = 0;
    virtual void resetDepths() = 0;
    virtual void addRealTimeInput(std::string nodeId) = 0;
    virtual void setRealTimeInputFlow(std::string nodeId, var_type flow) = 0;
    virtual var_type getRealTimeNodeHead(std::string nodeId) = 0;

    virtual var_type getNodeVariable(id_type nodeId, variables::Variables var) = 0;
    virtual void setNodeVariable(id_type nodeId, variables::Variables var, var_type value) = 0;

    virtual var_type getLinkVariable(id_type linkId, variables::Variables var) = 0;
    virtual void setLinkVariable(id_type linkId, variables::Variables var, var_type value) = 0;

    /// <summary>
    /// This function updates the given node statistic by adding the given value.
    /// </summary>
    virtual void updateNodeStatistic(id_type nodeId, statvariables::StatVariables var, var_type value) = 0;

    /// <summary>
    /// This function updates the given system statistic by adding the given value.
    /// </summary>
    virtual void updateSystemStatistic(statvariables::StatVariables var, var_type value) = 0;

    //virtual int getNodeCount() = 0;
    //virtual int getLinkCount() = 0;

    /// <summary>
    /// This function sets the given depth for the given node ID and then moves
    /// that depth to any upstream conduits.  Node depths can be different than
    /// upstream conduit depths because of transition losses in junctions or geometry changes.
    /// </summary>
    //virtual bool propagateNodeDepth(id_type nodeId, var_type depth) = 0;
};


#endif//MODEL_H__
