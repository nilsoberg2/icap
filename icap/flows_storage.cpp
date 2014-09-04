#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>

#include "../model/units.h"

#include "icap.h"
#include "routing.h"


#if defined(SWMM_GEOMETRY)
extern "C" double* NodeInflow;             // defined in MASSBAL.C
#endif


void ICAP::InitializeZeroFlows()
{
    m_geometry->resetDepths();
    m_geometry->resetTimestep();
}


void ICAP::InitializeZeroDepths()
{
    m_geometry->resetDepths();
}




// Returns the flow from all links connecting to the given node.  This
// is only useful for the downstream-most node (reservoir).
var_type ICAP::getFlowAtNode(const id_type& nodeIdx)
{
    return m_geometry->getNodeVariable(nodeIdx, variables::NodeFlow) + m_geometry->getNodeVariable(nodeIdx, variables::NodeLateralInflow);
}


var_type ICAP::computePipeStorage()
{
	double volume = 0.0;
    geometry::LinkList* links = m_geometry->getLinkList();
	for (int i = 0; i < links->count(); i++)
	{
		volume += m_geometry->getLinkVariable(links->id(i), variables::LinkVolume);
	}

	return volume;
}


double ICAP::computePondedPipeStorage(var_type h)
{
    //TODO: fix this function so that it works for Mainstream situations
    // as well (e.g. no adverse slope to reservoir but reservoir is on a
    // branch and one of the branches slopes down lower than the res.

    m_geometry->resetTimestep();

	// Set zero depth in the reservoir to compute ponded volume.
    m_geometry->setNodeVariable(m_sinkNodeIdx, variables::NodeDepth, h);

    // Route the small depth through the pipe network.
    steadyRoute(m_sinkNodeIdx, true); // true ==> ponded

	return computePipeStorage();
}


double ICAP::computePondedPipeStorage()
{
    return computePondedPipeStorage(m_geometry->getNode(m_sinkNodeIdx)->getInvert());
}


void ICAP::SaveTotalVolumeCurve(const std::string& file)
{
    using namespace std;

    var_type minX, minY, maxX, maxY;
    m_totalVolumeCurve.getFirstPoint(minX, minY);
    m_totalVolumeCurve.getLastPoint(maxX, maxY);

    ofstream stream;
    stream.open(file);

    if (!stream.good())
    {
        return;
    }

	stream << "STAGE-STORAGE CURVE" << endl;
    stream << "DEPTH\tVOLUME" << endl;

    var_type incs = 100;
    var_type inc = (maxY - minY) / incs;
    var_type curY = minY;

    while (curY <= (maxY + std::numeric_limits<double>::epsilon()))
    {
        double curX = m_totalVolumeCurve.inverseLookup(curY);
        stream << curY << curX << endl;
    }

    stream.close();
}


var_type ICAP::computeTotalVolumeCurve(geometry::Curve& curve)
{
    //TODO: fix this function so that it works for Mainstream situations
    // as well (e.g. no adverse slope to reservoir but reservoir is on a
    // branch and one of the branches slopes down lower than the res.

    geometry::NodeList* nodes = m_geometry->getNodeList();

    //double resInvert = GNODE_INVERT(m_sinkNodeIdx);
    //double pipeCrown = GLINK_MAXDEPTH(m_sinkLinkIdx) + GNODE_INVERT(m_sinkNodeIdx);
    var_type maxHeight = nodes->get(m_sinkNodeIdx)->getInvert() + nodes->get(m_sinkNodeIdx)->getMaxDepth();

    var_type maxVal = maxHeight;
    var_type minVal = 1000000.0;
    var_type incs = 100.0;
    var_type curStorage = 0.0;

    for (int i = 0; i < nodes->count(); i++)
    {
        minVal = std::min(nodes->get(i)->getInvert(), minVal);
    }

    // 50 increments
	var_type inc = (maxVal - minVal) / incs;
    var_type curElev = minVal;

    m_geometry->resetTimestep();

    while (curElev <= (maxVal + std::numeric_limits<double>::epsilon()))
    {
		//printf("%f\n", curElev);
	    // Set a very small depth to compute ponded volume.
	    m_geometry->setNodeVariable(m_sinkNodeIdx, variables::NodeDepth, curElev);

        // Route the small depth through the pipe network.
        steadyRoute(m_sinkNodeIdx, true); // true ==> ponded

        double temp = computePipeStorage();
        double temp2 = nodes->get(m_sinkNodeIdx)->lookupVolume(curElev - nodes->get(m_sinkNodeIdx)->getInvert());
        curStorage = temp + temp2 + 1e-6;  // + 1e-6 since we don't want it to be zero

        curve.addEntry(curStorage, curElev);

        curElev += inc;
    }

    if (curve.validate())
        return curStorage; // return the largest storage capacity.
    else
        return -1.0;
}


var_type ICAP::getSystemHead(var_type vol)
{
    return m_totalVolumeCurve.lookup(vol);
}


