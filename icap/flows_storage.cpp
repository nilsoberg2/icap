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


