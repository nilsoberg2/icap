#ifndef MODEL_H__
#define MODEL_H__

#include <string>
#include "../time/datetime.h"
#include "variables.h"
#include "../api.h"


class ICAP_API IModel
{
public:
    virtual void resetTimestep(DateTime dateTime) = 0;
    virtual void startTimestep(DateTime dateTime) = 0;
    virtual DateTime getCurrentDateTime() = 0;
    virtual void resetDepths() = 0;
    virtual void addRealTimeInput(std::string nodeId) = 0;
    virtual void setRealTimeInputFlow(std::string nodeId, double flow) = 0;
    virtual double getRealTimeNodeHead(std::string nodeId) = 0;

    virtual var_type getNodeVariable(std::string nodeId, variables::Variables var) = 0;
    virtual var_type getLinkVariable(std::string linkId, variables::Variables var) = 0;

    /// <summary>
    /// This function sets the given depth for the given node ID and then moves
    /// that depth to any upstream conduits.  Node depths can be different than
    /// upstream conduit depths because of transition losses in junctions or geometry changes.
    /// </summary>
    virtual bool propagateNodeDepth(std::string nodeId, double depth) = 0;
};


#endif//MODEL_H__
