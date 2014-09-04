#define _CRT_SECURE_NO_DEPRECATE

#define _USE_MATH_DEFINES
#include <cmath>
#include <map>
#include <vector>

#include "../hpg/error.hpp"
#include "../util/math.h"

#include "routing.h"
#include "icap.h"
#include "hpg.h"
#include "benchmark.h"


bool ICAP::steadyRoute(const id_type& sinkNodeIdx, bool ponded)
{
    using namespace std;

    var_type flow = m_geometry->getNodeVariable(sinkNodeIdx, variables::NodeFlow);
    m_stepCount++;

    map<id_type, bool> followList;
    vector<id_type> toFollow;

    toFollow.push_back(sinkNodeIdx);

    bool toContinue = true;

    while (! toFollow.empty())
    {
        id_type nodeId = toFollow.back();
        toFollow.pop_back();

        // Skip nodes already processed.
        if (followList.find(nodeId) != followList.end())
            continue;

        // Get the node object associated with the index.
        std::shared_ptr<geometry::Node> node = m_geometry->getNode(nodeId);

        //if (ponded)
        //    toContinue = pondedRouteNode(nodeId);
        //else
            toContinue = steadyRouteNode(nodeId, ponded);

        if (! toContinue)
            break;

        for (auto link: node->getUpstreamLinks())
        {
            if (ponded)
                toContinue = pondedRouteLink(link->getId());
            else
                toContinue = steadyRouteLink(link->getId());

            if (! toContinue)
                break;

            toFollow.push_back(link->getUpstreamNode()->getId());
        }

        if (! toContinue)
            break;

        followList.insert(std::make_pair(nodeId, true));
    }

    return toContinue;
}


/// <summary>
/// The goal of this function is to pass a node depth to the downstream end
/// of upstream conduits.  Node depths can be different than conduit depths
/// because of transition losses in junctions or geometry changes.
/// </summary>
bool ICAP::steadyRouteNode(const id_type& nodeId, bool isPonded)
{
    BOOST_LOG_SEV(m_log, loglevel::debug) << "Routing node " << nodeId;

    bool okToContinue = true;

    std::shared_ptr<geometry::Node> node = m_geometry->getNode(nodeId);
    var_type flow = m_geometry->getNodeVariable(nodeId, variables::NodeFlow);
    var_type depth = m_geometry->getNodeVariable(nodeId, variables::NodeDepth);
    
    double downDiam = node->getDownstreamLinkMaxDepth();
    if (downDiam < 0)
    {
        downDiam = 0;
    }

    // Look at every upstream node and carry the elevation upstream.
    // If the elevation is less than the invert of the upstream node,
    // then we just set their elevation to their invert elevation.
    // This is only done if there is flow in the pipes.
    bool geomChanges = false;

    for (auto link: node->getUpstreamLinks())
    {
        if (isPonded)
        {
            link->variable(variables::LinkDsDepth) = depth;
        }
        else
        {
            var_type dsInvert = link->getDownstreamInvert();
            if (!isZero(depth) && depth + node->getInvert() > dsInvert)
            {
                link->variable(variables::LinkDsDepth) = depth - dsInvert;
            }
            else
            {
                link->variable(variables::LinkDsDepth) = 0;
            }
        
            if (link->getGeometryType() != xs::xstype::dummy && !isZero(downDiam) && link->getMaxDepth() != downDiam)
            {
                geomChanges = true;
            }
        }
    }

    // If we're not calculating junction losses, we just return at
    // this point.  We've already carried the junctions stage to
    // the upstream nodes in the last loop.  We also return if there
    // aren't any downstream or upstream nodes because there are no
    // losses to calculate, and return if none of the upstream pipes
    // have flow in them.  We also return if there is only one
    // upstream pipe and there is no change in geometry.
    int degree = node->getDownstreamLinks().size() + node->getUpstreamLinks().size();
    if (isPonded || degree < 2 || isZero(flow) || (degree == 2 && !geomChanges))
    {
        return true;
    }
    else
    {
        return computeNodeLosses(nodeId);
    }
}

//
//bool ICAP::pondedRouteNode(int nodeId)
//{
//    //dprintf("Routing ponded node %d", nodeId);
//
//    bool okToContinue = true;
//
//    // Get the node from our m_network representation.
//    ICAPNode* node = m_network.FindNode(nodeId);
//    if (node == NULL)
//    {
//        char temp[25];
//        sprintf(temp, "%d", nodeId);
//        m_errorStr += "Unable to find node ";
//        m_errorStr += temp;
//        m_errorStr += " in steadyRouteNode.";
//        return false;
//    }
//
//    int degree = (int)node->linkId.size();
//
//    double flow = m_geometry->getNodeVariable(nodeId, variables::NodeInflow);
//    double depth = GNODE_DEPTH(nodeId);
//
//    // Look at every upstream node and carry the elevation upstream.
//    for (int i = 0; i < degree; i++)
//    {
//        int linkId = node->linkId.at(i);
//
//        // Only look at nodes that are upstream of this node.
//        if (GLINK_DSNODE(linkId) != nodeId)
//            continue;
//
//        updateUpstreamElevationForNode(linkId, depth);
//    }
//
//    // After passing the elevation to the upstream pipes, convert the
//    // water elevation to a water depth for this node.
//    GNODE_DEPTH(nodeId) = elevationToDepthForNode(nodeId);
//
//    return true;
//}
//


bool ICAP::steadyRouteLink(const id_type& linkId)
{
    BOOST_LOG_SEV(m_log, loglevel::debug) << "Routing link " << linkId;

    bool okToContinue = true;

    std::shared_ptr<geometry::Link> link = m_geometry->getLink(linkId);

    var_type flow = link->variable(variables::LinkFlow);
    var_type dsDepth = link->variable(variables::LinkDsDepth);
    var_type usDepth = 0;
	var_type volume = 0.0;
	var_type slope = link->getSlope();
    var_type length = link->getLength();
    var_type dsInvert = link->getDownstreamInvert();
    var_type usInvert = link->getUpstreamInvert();

    // Carry the depth across if the link isn't a conduit or doesn't have
    // proper geometry.
    if (link->getGeometryType() == xs::xstype::dummy)
    {
        link->variable(variables::LinkVolume) = 0;

        if (!isZero(dsDepth))
        {
            link->variable(variables::LinkUsDepth) = std::max(0.0, dsDepth - slope * length);
        }

        return true;
    }

	if (slope < 0.0)
    {
		flow = -flow;
    }

    //var_type minFlow = m_hpgList.getLowestFlow(linkId, flow < 0);
    //
    //// If there isn't any flow, then we carry the current water
    //// elevation across to upstream nodes (ponding).
    //if (fabs(flow) < fabs(minFlow))
    //{
    //    // We only want to carry across the ponding effect if the
    //    // ponding will actually be above the upstream tunnel invert.
    //    if (!isZero(dsDepth) && dsDepth + dsInvert > usInvert)
    //    {
    //        usDepth = dsDepth - slope * length;
    //    }
    //    // If the water isn't high enough, then we mark this pipe
    //    // as having no water at the upstream end.
    //    else
    //    {
    //        usDepth = 0.0;
    //    }
	//    volume = link->computeLevelVolume(dsDepth);
    //}
    //// In this case there is flow.  Interpolate from our HPGs now.
    //else
    //{
    if (!m_hpgList.getUpstream(linkId, dsDepth + dsInvert, flow, usDepth))
    {
        BOOST_LOG_SEV(m_log, loglevel::error) << "Unable to query the HPG for upstream using the parameters dsDepth=" << 
            dsDepth << " flow=" << flow << " link=" << m_geometry->getLink(linkId)->getName() << "; error: " << m_hpgList.getErrorMessage();
        okToContinue = false;
    }

    if (okToContinue && !m_hpgList.getVolume(linkId, dsDepth + dsInvert, flow, volume))
    {
        BOOST_LOG_SEV(m_log, loglevel::error) << "Unable to query the HPG for volume using the parameters dsDepth=" << 
            dsDepth << " flow=" << flow << " link=" << m_geometry->getLink(linkId)->getName() << "; error: " << m_hpgList.getErrorMessage();
        okToContinue = false;
    }
	//}

    // If there wasn't an error, then we carry the interpolated upstream
    // elevation to the upstream nodes.
    if (okToContinue)
    {
        link->variable(variables::LinkUsDepth) = usDepth - usInvert;
        auto node = link->getUpstreamNode();
        node->variable(variables::NodeDepth) = usDepth - node->getInvert();
        link->variable(variables::LinkVolume) = volume;
        return true;
    }
    else
        return false;
}


bool ICAP::pondedRouteLink(const id_type& linkId)
{
    BOOST_LOG_SEV(m_log, loglevel::debug) << "Routing ponded link " << linkId;

    std::shared_ptr<geometry::Link> link = m_geometry->getLink(linkId);
    
    double dsElev = link->variable(variables::LinkDsDepth); // returns elevation in ponded case
	double volume = 0.0;
    double dsDepth = dsElev - link->getDownstreamInvert();

    // If the link is a DUMMY link, then the volume is zero.
    if (link->getGeometryType() == xs::xstype::dummy)
    {
        volume = 0.0;
    }
    else
    {
	    volume = link->computeLevelVolume(dsDepth);
    }

    link->variable(variables::LinkUsDepth) = dsElev;
    link->getUpstreamNode()->variable(variables::NodeDepth) = dsElev;
    link->variable(variables::LinkVolume) = volume;

    // Set the downstream value to be stored to be DEPTH, not elevation.
    if (dsDepth < 0.0)
        link->variable(variables::LinkDsDepth) = 0;
    else
        link->variable(variables::LinkDsDepth) = dsDepth;

    return true;
}
