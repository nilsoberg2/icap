
#include "icap.h"


void ICAP::SetCurrentNodeInflow(const std::string& nodeId, var_type flow)
{
    m_geometry->setRealTimeInputFlow(nodeId, flow);
}


void ICAP::SetCurrentNodeHead(const std::string& nodeId, var_type head)
{
    m_realTimeDsHead = true;
    m_geometry->setRealTimeInputHead(nodeId, head);
}


var_type ICAP::GetCurrentNodeHead(const std::string& nodeId)
{
    return m_geometry->getRealTimeNodeHead(nodeId);
}


var_type ICAP::GetCurrentNodeInflow(const std::string& nodeId)
{
    return m_geometry->getRealTimeNodeInflow(nodeId);
}


void ICAP::AddSource(const std::string& nodeId)
{
    m_geometry->addRealTimeInput(nodeId);
}


void ICAP::SetFlowFactor(var_type flowFactor)
//
//  Input:   flow factor
//  Output:  none
//  Purpose: modifies scale factor on inflows
//
{
    for (geometry::Geometry::NodeIter iter = m_geometry->beginNode(); iter != m_geometry->endNode(); iter++)
    {
        iter->second->setInflowFactor(flowFactor);
    }
}


void ICAP::EnableRealTimeStatus()
{
    m_realTimeFlows = true;
    if (m_geometry != NULL)
    {
        m_geometry->enableRealTimeStatus();
    }
}
