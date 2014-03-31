#define _CRT_SECURE_NO_DEPRECATE
#define _USE_MATH_DEFINES

#include "routing.h"
#include "icap.h"
#include "hpg.h"
#include "debug.h"
#include <cmath>
#include <hpg/error.hpp>
#include <map>
#include <vector>
#include "benchmark.h"


bool ICAP::steadyRoute(int sinkNodeIdx, bool ponded)
{
    double flow = Node[sinkNodeIdx].inflow;
    StepCount++;

    using namespace std;
    map<int, bool> followList;
    vector<int> toFollow;

    toFollow.push_back(sinkNodeIdx);
    
    bool toContinue = true;

    while (! toFollow.empty())
    {
        int nodeIdx = toFollow.back();
        toFollow.pop_back();

        // Skip nodes already processed.
        if (followList.find(nodeIdx) != followList.end())
            continue;

        // Get the node object associated with the index.
        ICAPNode* node = m_network.FindNode(nodeIdx);
        // Commented out this code because the network shouldn't have an
        // invalid pointer.  If it does, then program should crash and we
        // shouldn't handle it here.
        //if (node == NULL)
        //{
        //    error("Invalid pointer found in m_network in icap_steadyRoute.");
        //    exit(1);
        //}

        if (ponded)
            toContinue = pondedRouteNode(nodeIdx);
        else
            toContinue = steadyRouteNode(nodeIdx);

        if (! toContinue)
            break;

        for (unsigned int i = 0; i < node->linkIdx.size(); i++)
        {
            int id = node->linkIdx.at(i);
            if (Link[id].node2 == nodeIdx)
            {
                if (ponded)
                    toContinue = pondedRouteLink(id);
                else
                    toContinue = steadyRouteLink(id);

                if (! toContinue)
                    break;

                toFollow.push_back(Link[id].node1);
            }
        }

        if (! toContinue)
            break;

        followList.insert(std::make_pair(nodeIdx, true));
    }

    return toContinue;
}


bool ICAP::steadyRouteNode(int nodeIdx)
{
    //dprintf("Routing node %d", nodeIdx);

    bool okToContinue = true;

    // Get the node from our m_network representation.
    ICAPNode* node = m_network.FindNode(nodeIdx);
    if (node == NULL)
    {
        char temp[25];
        sprintf(temp, "%d", nodeIdx);
        m_errorStr += "Unable to find node ";
        m_errorStr += temp;
        m_errorStr += " in steadyRouteNode.";
        return false;
    }

    int degree = (int)node->linkIdx.size();

    // The first step is to get the diameter of the downstream pipe.
    double downDiam = 0.0;
    if (degree > 1)
    {
        for (int i = 0; i < degree; i++)
        {
            int linkIdx = node->linkIdx.at(i);
            if (Link[linkIdx].node1 == nodeIdx &&       // upstream of the current link is the node that we're routing
                Link[linkIdx].type == CONDUIT &&        // the link is a conduit
                Link[linkIdx].xsect.type == CIRCULAR)   // the link cross-section is circular
            {
                downDiam = Link[linkIdx].xsect.yFull;
                break;
            }
        }
    }

    double flow = Node[nodeIdx].inflow;
    double depth = Node[nodeIdx].newDepth;

    // Look at every upstream node and carry the elevation upstream.
    // If the elevation is less than the invert of the upstream node,
    // then we just set their elevation to their invert elevation.
    // This is only done if there is flow in the pipes.
    bool geomChanges = false;
    for (int i = 0; i < degree; i++)
    {
        int linkIdx = node->linkIdx.at(i);

        // Only look at nodes that are upstream of this node.
        if (Link[linkIdx].node2 != nodeIdx)
            continue;

		// Process the reservoir interface differently.  If the critical depth at
		// the pipe outlet is greater than the depth in the reservoir, then we
		// set the downstream depth in the pipe to be equal to the critical depth.
        // We do this here because we don't want to affect the actual reservoir
        // depth that is stored in the Node object.
		if (IS_NOT_ZERO(flow) && nodeIdx == m_sinkNodeIdx)
		{
			double resElev = depth + Node[nodeIdx].invertElev;
			
            double yCrit = 0;
			double pipeDSDepth = 0;
            if (!m_hpgList.CalculateCriticalDepth(flow, LINK_DIAMETER(linkIdx), LINK_DIAMETER(linkIdx) / 2, yCrit))
            {
                pipeDSDepth = MAX(LINK_DS_INVERT(linkIdx) + LINK_DIAMETER(linkIdx), resElev) - LINK_DS_INVERT(linkIdx) + Link[linkIdx].offset2;
            }
            else
            {
    			double critElev = LINK_DS_INVERT(linkIdx) + yCrit;
			    pipeDSDepth = MAX(critElev, resElev) - LINK_DS_INVERT(linkIdx) + Link[linkIdx].offset2;
            }

			updateUpstreamDepthForNode(linkIdx, pipeDSDepth);
		}
        else if (IS_NOT_ZERO(depth) && depth+Node[nodeIdx].invertElev > LINK_DS_INVERT(linkIdx))
            updateUpstreamDepthForNode(linkIdx, depth);
        else
            updateUpstreamDepthForNode(linkIdx, 0.0);
        
        if (Link[linkIdx].type == CONDUIT &&
            Link[linkIdx].xsect.type == CIRCULAR &&
            IS_NOT_ZERO(downDiam) &&
            Link[linkIdx].xsect.yFull != downDiam)
          geomChanges = true;
    }

    // If we're not calculating junction losses, we just return at
    // this point.  We've already carried the junctions stage to
    // the upstream nodes in the last loop.  We also return if there
    // aren't any downstream or upstream nodes because there are no
    // losses to calculate, and return if none of the upstream pipes
    // have flow in them.  We also return if there is only one
    // upstream pipe and there is no change in geometry.
    if (degree < 2 || IS_ZERO(flow) || (degree == 2 && ! geomChanges))
        return true;
    else
        return computeNodeLosses(nodeIdx, node);
}


bool ICAP::pondedRouteNode(int nodeIdx)
{
    //dprintf("Routing ponded node %d", nodeIdx);

    bool okToContinue = true;

    // Get the node from our m_network representation.
    ICAPNode* node = m_network.FindNode(nodeIdx);
    if (node == NULL)
    {
        char temp[25];
        sprintf(temp, "%d", nodeIdx);
        m_errorStr += "Unable to find node ";
        m_errorStr += temp;
        m_errorStr += " in steadyRouteNode.";
        return false;
    }

    int degree = (int)node->linkIdx.size();

    double flow = Node[nodeIdx].inflow;
    double depth = Node[nodeIdx].newDepth;

    // Look at every upstream node and carry the elevation upstream.
    for (int i = 0; i < degree; i++)
    {
        int linkIdx = node->linkIdx.at(i);

        // Only look at nodes that are upstream of this node.
        if (Link[linkIdx].node2 != nodeIdx)
            continue;

        updateUpstreamElevationForNode(linkIdx, depth);
    }

    // After passing the elevation to the upstream pipes, convert the
    // water elevation to a water depth for this node.
    Node[nodeIdx].newDepth = elevationToDepthForNode(nodeIdx);

    return true;
}


double ICAP::elevationToDepthForNode(int nodeIdx)
{
    double result = Node[nodeIdx].newDepth - Node[nodeIdx].invertElev;
    if (result < 0.0)
        return 0.0;
    else
        return result;
}


double ICAP::elevationToDSDepthForLink(int linkIdx)
{
    double result = LINK_DS_DEPTH(linkIdx) - LINK_DS_INVERT(linkIdx);
    if (result < 0.0)
        return 0.0;
    else
        return result;
}


double ICAP::elevationToUSDepthForLink(int linkIdx)
{
    double result = LINK_US_DEPTH(linkIdx) - LINK_US_INVERT(linkIdx);
    if (result < 0.0)
        return 0.0;
    else
        return result;
}


void ICAP::updateUpstreamDepthForNode(int linkIdx, double dsDepth)
{
    // Set depth on downstream end of pipe.
    LINK_DS_DEPTH(linkIdx) = dsDepth - Link[linkIdx].offset2;

    if (LINK_DS_DEPTH(linkIdx) < 0.0)
        LINK_DS_DEPTH(linkIdx) = 0.0;
}


void ICAP::updateUpstreamElevationForNode(int linkIdx, double dsElev)
{
    // Set elevation on downstream end of pipe.
    LINK_DS_DEPTH(linkIdx) = dsElev;
}


void ICAP::updateUpstreamDepthForLink(int linkIdx, double usDepth, double volume)
{
    // Set depth on upstream end of pipe.
    LINK_US_DEPTH(linkIdx) = usDepth;

    if (LINK_US_DEPTH(linkIdx) < 0.0)
        LINK_US_DEPTH(linkIdx) = 0.0;

	Link[linkIdx].newVolume = volume;

    // Update the depth for the node upstream.
    int usNodeIdx = Link[linkIdx].node1;
    Node[ usNodeIdx ].newDepth = usDepth + Link[linkIdx].offset1;

    if (Node[ usNodeIdx ].newDepth < 0.0)
        Node[ usNodeIdx ].newDepth = 0.0;
    else if (IS_ZERO(LINK_US_DEPTH(linkIdx)))
        Node[ usNodeIdx ].newDepth = 0.0;
}


void ICAP::updateUpstreamElevationForLink(int linkIdx, double usElev, double volume)
{
    // Set DEPTH, not elevation, on upstream end of pipe.
    LINK_US_DEPTH(linkIdx) = usElev - LINK_US_INVERT(linkIdx);

	Link[linkIdx].newVolume = volume;

    // Update the ELEVATION, not depth, for the node upstream.
    int usNodeIdx = Link[linkIdx].node1;
    Node[ usNodeIdx ].newDepth = usElev;
}


bool ICAP::steadyRouteLink(int linkIdx)
{
    //dprintf("Routing link %d", linkIdx);

    BENCH_INIT;
    
    bool okToContinue = true;

    double flow = Link[linkIdx].newFlow;
    double usDepth = LINK_US_DEPTH(linkIdx);//Link[linkIdx].newDepth;
    double dsDepth = LINK_DS_DEPTH(linkIdx);//Link[linkIdx].oldDepth;
	double volume = 0.0;
	double slope = LINK_SLOPE(linkIdx);

    // Carry the depth across if the link isn't a conduit or doesn't have
    // proper geometry.
    if (Link[linkIdx].type != CONDUIT || ! LINK_HASGEOM(linkIdx))
    {
        Link[linkIdx].newVolume = 0.0;
        if (IS_ZERO(dsDepth))
            return true;//updateUpstreamDepthForLink(linkIdx, 0.0);
        else
        {
            updateUpstreamDepthForLink(linkIdx, dsDepth - LINK_SLOPE(linkIdx) * LINK_LENGTH(linkIdx), 0.0);
            return true;
        }
    }

	if (slope < 0.0)
		flow = -flow;
    
    // If there isn't any flow, then we carry the current water
    // elevation across to upstream nodes (ponding).
    if (IS_ZERO(flow))
    {
		//DEBUG_MSG2("\t0");
        // We only want to carry across the ponding effect if the
        // ponding will actually be above the upstream tunnel invert.
        if (IS_NOT_ZERO(dsDepth) && dsDepth+LINK_DS_INVERT(linkIdx) > LINK_US_INVERT(linkIdx))
            usDepth = dsDepth - LINK_SLOPE(linkIdx) * LINK_LENGTH(linkIdx);

        // If the water isn't high enough, then we mark this pipe
        // as having no water.
        else
            usDepth = 0.0;

		volume = ComputePipeVolume(LINK_DIAMETER(linkIdx), LINK_SLOPE(linkIdx),
			LINK_LENGTH(linkIdx), dsDepth - LINK_SLOPE(linkIdx) * LINK_LENGTH(linkIdx));
    }

    // Ok, now there is flow.  So we check to see if that flow is in
    // the range of the HPG AND there are valid curves with which to
    // do interpolation.
    else
	{
        double tempFlow1 = m_hpgList.GetLowestFlow(linkIdx, flow);
        if (fabs(flow) <= fabs(tempFlow1))
            flow = tempFlow1 * 1.05;

        BENCH_START;

		int interpOK = m_hpgList.CanInterpolateExtended(linkIdx, dsDepth, flow);
		int hpgEC = m_hpgList.GetErrorCode();

        BENCH_STOP;

        #if defined(BENCHMARKYES)
        // We do ifdef here because we don't want std::cout present unless benchmarking
        BENCH_REPORT_IOS("InterpInit ");
        #endif

		// If there was an error then we bomb out.
		if (interpOK == HPG_ERROR)
			okToContinue = false;

		// Pipe empty or flow/depth is valid for HPG.
		else if (hpgEC == 0 && (interpOK == HPG_PIPE_EMPTY || interpOK == HPG_PIPE_OK))
		{
			//DEBUG_MSG2("\t1");

            BENCH_START;

			// Get the upstream value (either use critical if less than the
			// invert or the actual curve).  This will return a depth relative
			// to the pipe invert.
			bool status = m_hpgList.GetUpstream(linkIdx, dsDepth, flow, usDepth);

            BENCH_STOP;

            #if defined(BENCHMARKYES)
            // We do ifdef here because we don't want std::cout present unless benchmarking
            BENCH_REPORT_IOS("GetUpstream ");
            #endif

			// Check to see if there was an error.
			if (! status)
			{
				char temp[50];
				sprintf(temp, "link=%s dsDepth=%f flow=%f", Link[linkIdx].ID, dsDepth, flow);
				m_errorStr += "HPG interpolation for ";
				m_errorStr += temp;
				m_errorStr += " failed in steadyRouteLink (HPG error: ";
				m_errorStr += m_hpgList.GetErrorStr();
				m_errorStr += ").";
				okToContinue = false;
			}
			
            BENCH_START;

			// Get the volume stored in the pipe
			status = m_hpgList.GetVolume(linkIdx, dsDepth, flow, volume);

            BENCH_STOP;

            #if defined(BENCHMARKYES)
            // We do ifdef here because we don't want std::cout present unless benchmarking
            BENCH_REPORT_IOS("GetVolume ");
            #endif

			// Check to see if there was an error.
			if (! status)
			{
				char temp[50];
				sprintf(temp, "link=%s dsDepth=%f flow=%f", Link[linkIdx].ID, dsDepth, flow);
				m_errorStr += "HPG volume interpolation for ";
				m_errorStr += temp;
				m_errorStr += " failed in steadyRouteLink (HPG error: ";
				m_errorStr += m_hpgList.GetErrorStr();
				m_errorStr += ").";
				okToContinue = false;
			}
		}

		// Ok.  So now we have flow, but it isn't in the interpolation range.
		// So we handle this with our special interpolation routines.
		else if (hpgEC == 0 && interpOK == HPG_PIPE_FULL)
		{
			//DEBUG_MSG2("\t2");

            BENCH_START;

			// We do our pressurized flow calculations, since the data are
			// outside of interpolable regions.  This will return a depth relative
			// to the pipe invert.
			bool status = m_hpgList.PressurizedInterpolation(linkIdx, dsDepth, flow, usDepth);

            BENCH_STOP;

            #if defined(BENCHMARKYES)
            // We do ifdef here because we don't want std::cout present unless benchmarking
            BENCH_REPORT_IOS("PressurizedInterpolation ");
            #endif


			// Check to see if there was an error.
			if (! status)
			{
				char temp[50];
				sprintf(temp, "link=%d dsDepth=%f flow=%f", linkIdx, dsDepth, flow);
				m_errorStr += "Pressurized computation for ";
				m_errorStr += temp;
				m_errorStr += " failed in steadyRouteLink.";
				okToContinue = false;
			}

			double rad = LINK_DIAMETER(linkIdx) / 2.0;
			volume = M_PI * rad * rad * LINK_LENGTH(linkIdx);
		}

		else
		{
			char temp[50];
			sprintf(temp, "%d dsDepth=%f flow=%f", linkIdx, dsDepth, flow);
			m_errorStr += "A HPG error occurred (";
			m_errorStr += m_hpgList.GetErrorStr();
			m_errorStr += ") in steadyRouteLink.";
			okToContinue = false;
		}
	}

    // If there wasn't an error, then we carry the interpolated upstream
    // elevation to the upstream nodes.
    if (okToContinue)
    {
        updateUpstreamDepthForLink(linkIdx, usDepth, volume);
        return true;
    }
    else
        return false;
}


bool ICAP::pondedRouteLink(int linkIdx)
{
    //dprintf("Routing ponded link %d", linkIdx);
    
    double dsElev = LINK_DS_DEPTH(linkIdx); // returns elevation in ponded case
	double volume = 0.0;
    double dsDepth = dsElev - LINK_DS_INVERT(linkIdx);

    // If the link is a DUMMY link, then the volume is zero.
    if (Link[linkIdx].type != CONDUIT || ! LINK_HASGEOM(linkIdx))
        volume = 0.0;
    else
    {
        double S = LINK_SLOPE(linkIdx);
        double L = LINK_LENGTH(linkIdx);
        double usDepth = dsDepth - S * L;
		double D = Link[linkIdx].xsect.yFull;
	    volume = ComputePipeVolume(D, S, L, usDepth);
    }

    updateUpstreamElevationForLink(linkIdx, dsElev, volume);

    // Set the downstream value to be stored to be DEPTH, not elevation.
    if (dsDepth < 0.0)
        LINK_DS_DEPTH(linkIdx) = 0.0;
    else
        LINK_DS_DEPTH(linkIdx) = dsDepth;

    return true;
}
