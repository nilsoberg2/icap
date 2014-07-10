
#include "icap.h"


void ICAP::SetCurrentNodeInflow(char* nodeId, double flow)
{
    m_model->setRealTimeInputFlow(nodeId, flow);
}


double ICAP::GetCurrentNodeHead(char* nodeId)
{
    return m_model->getRealTimeNodeHead(nodeId);
}


void ICAP::AddSource(char* nodeId)
{
    m_model->addRealTimeInput(nodeId);
}


void ICAP::SetFlowFactor(double flowFactor)
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
