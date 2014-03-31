#define _USE_MATH_DEFINES

#include "icap.h"
#include "routing.h"
#include <cmath>
#include <vector>


extern "C" double* NodeInflow;             // defined in MASSBAL.C


void ICAP::InitializeZeroFlows()
{
    // Initialize the inflows for all nodes that have them and save
    // the old flows for each node.
    for (int j = 0; j < Nobjects[NODE]; j++)
    {
        Node[j].oldLatFlow  = Node[j].newLatFlow;
        Node[j].newLatFlow  = 0.0;
        Node[j].inflow = Node[j].outflow = 0.0;
        Node[j].oldDepth = Node[j].newDepth;
    }

    // Save the old flows for each link.
    for (int j = 0; j < Nobjects[LINK]; j++)
    {
        Link[j].oldFlow = Link[j].newFlow;
        Link[j].newFlow = 0.0;
		Link[j].oldVolume = Link[j].newVolume;
		Link[j].newVolume = 0.0;
        //link_setOldHydState(j);
    }
}


void ICAP::InitializeZeroDepths()
{
    // Initialize the inflows for all nodes that have them and save
    // the old flows for each node.
    for (int j = 0; j < Nobjects[NODE]; j++)
    {
        Node[j].oldDepth = Node[j].newDepth;
        Node[j].newDepth = 0.0;
    }

    // Save the old flows for each link.
    for (int j = 0; j < Nobjects[LINK]; j++)
    {
		Link[j].oldDepth = Link[j].newDepth;
		Link[j].newDepth = 0.0;
		Link[j].oldVolume = Link[j].newVolume;
		Link[j].newVolume = 0.0;
    }
}


void ICAP::SetCurrentNodeInflow(char* nodeId, double flow)
{
    int index = FindNodeIndex(nodeId);
    if (index < 0)
        return;

    Node[index].newLatFlow = flow;
}


double ICAP::GetCurrentNodeHead(char* nodeId)
{
    int index = FindNodeIndex(nodeId);
    if (index < 0)
        return ERROR_VAL;

    return Node[index].newDepth + Node[index].invertElev;
}


void ICAP::AddSource(char* nodeId)
{
    int index = FindNodeIndex(nodeId);
    if (index < 0)
        return;
    ICAPNode* node = m_network.FindNode(index);
    
    for (int i = 0; i < node->linkIdx.size(); i++)
    {
        Source temp;
        temp.link = i;
        temp.active = false;

        // We only pick sources that are upstream of the current link.
        if (Link[node->linkIdx.at(i)].node1 == index)
        {
            temp.node = index;
            temp.active = true;
        }

        if (temp.active)
            m_sources.AddSource(temp);
    }
}


void ICAP::initializeFlows(double curDate)
{
    // Initialize the inflows for all nodes that have them and save
    // the old flows for each node.
    for (int j = 0; j < Nobjects[NODE]; j++)
    {
        Node[j].oldLatFlow  = Node[j].newLatFlow;
        Node[j].newLatFlow  = 0.0;
        Node[j].inflow = Node[j].outflow = 0.0;
        Node[j].oldDepth = Node[j].newDepth;
    }

    addExternalInflows(curDate);

    // Save the old flows for each link.
    for (int j = 0; j < Nobjects[LINK]; j++)
    {
        Link[j].oldFlow = Link[j].newFlow;
        Link[j].newFlow = 0.0;
		Link[j].oldVolume = Link[j].newVolume;
		Link[j].newVolume = 0.0;
        //link_setOldHydState(j);
    }
}


void ICAP::propogateFlows()
{
    // Instantaneously propogate the flow downstream, from each source to the sink.
    int srcCount = m_sources.Count();
    for (int s = 0; s < srcCount; s++)
    {
        Source src = m_sources.GetSource(s);
        if (src.active)
            setFlowsFromSource(src.node, src.link);
    }
}


void ICAP::setFlowsFromSource(int node, int link)
{
    if (IS_ZERO(Node[node].newLatFlow))
        return;

    double flow = Node[node].newLatFlow;
	Node[node].inflow += flow;
	NodeInflow[node] += flow * RouteStep;

    using namespace std;
    map<int, bool> followList;
    vector<int> toFollow;

    toFollow.push_back(node);

    while (! toFollow.empty())
    {
        int id = toFollow.back();
        toFollow.pop_back();

        if (followList.find(id) != followList.end())
            continue;

        ICAPNode* ptr = m_network.FindNode(id);
        // Commented this out because the network should never have an
        // invalid pointer.  If it does, the program should probably crash
        // instead of handling the error here.
        //if (ptr == NULL)
        //{
        //    error("Invalid pointer found in m_network in icap_setFlowsFromSource.");
        //    exit(1);
        //}

        for (unsigned int i = 0; i < ptr->linkIdx.size(); i++)
        {
            int linkID = ptr->linkIdx.at(i);

            // We only add to the follow list if the pipe we're looking at is
            // downstream of the current junction (as identified by id).
            if (Link[linkID].node2 != id)
            {
                Link[linkID].newFlow += flow;
                Node[ Link[linkID].node2 ].inflow += flow;
                NodeInflow[ Link[linkID].node2 ] += flow * RouteStep;
                toFollow.push_back(Link[linkID].node2);
            }
        }

        followList.insert(std::make_pair(id, true));
    }
}


void ICAP::addExternalInflows(DateTime currentDate)
//
//  Input:   currentDate = current date/time
//  Output:  none
//  Purpose: adds direct external inflows to nodes at current date.
//           Trimmed down version of addExternalInflows in routing.c
//
{
    int     j;
    double  q;
    TExtInflow* inflow;

    // --- for each node with a defined external inflow
    for (j = 0; j < Nobjects[NODE]; j++)
    {
        inflow = Node[j].extInflow;
        if ( !inflow ) continue;

        // --- get flow inflow
        q = 0.0;
        while ( inflow )
        {
            if ( inflow->type == FLOW_INFLOW )
            {
                q = inflow_getExtInflow(inflow, currentDate);
            }
            inflow = inflow->next;
        }
        if ( fabs(q) < FLOW_TOL ) q = 0.0;

        // --- add flow inflow to node's lateral inflow
        Node[j].newLatFlow += q;
		massbal_addInflowFlow(EXTERNAL_INFLOW, q);
    }
}


void ICAP::SetFlowFactor(float flowFactor)
//
//  Input:   flow factor
//  Output:  none
//  Purpose: modifies scale factor on inflows
//
{
    int     j;
    double  q;
    TExtInflow* inflow;

    // --- for each node with a defined external inflow
    for (j = 0; j < Nobjects[NODE]; j++)
    {
        inflow = Node[j].extInflow;
        if ( !inflow ) continue;

        inflow->sFactor = flowFactor;
    }
}



// Returns the flow from all links connecting to the given node.  This
// is only useful for the downstream-most node (reservoir).
double ICAP::getFlowAtNode(int idx)
{
    ICAPNode* node = m_network.FindNode(idx);
    if (node == NULL)
        return 0.0;

    // Look at each connecting link and add the flow.
    double flow = 0.0;
    for (unsigned int i = 0; i < node->linkIdx.size(); i++)
    {
        int linkIdx = node->linkIdx.at(i);
        flow += Link[linkIdx].newFlow;
    }

	flow += Node[idx].newLatFlow;

    return flow;
}


double ICAP::computePumpedVolume(double flowAtRes, double currentDate)
{
    // If we have a pump timeseries, then return that volume.
    if (m_pumpTSIdx >= 0)
        return getPumpedVolume(currentDate);
    
    // Otherwise do the static pumping rate computation.
    
	// If the flow at the reservoir is less than a given threshold, we
	// assume that no flow is entering and we start counting the seconds
	// since no flow has entered.  If Q at reservoir is >= the threshold
	// then we reset the counter to disable pumping.
    if (flowAtRes > PumpingThreshold)
        m_secondsSinceLastInflow = 0.0;
    else
    	m_secondsSinceLastInflow += RouteStep;

	// If the last inflow counter is more than two days old, then we start
	// pumping out of the reservoir.
	if (m_secondsSinceLastInflow >= SECS_PER_DAY * DaysBeforePumping) // # days (in seconds) since flow dropped below threshold
        return PumpingRate * UCF(FLOW) * RouteStep;
    else
        return 0.0;
}


double ICAP::getPumpedRate(double date)
{
    if (m_pumpTSIdx >= 0)
        return table_tseriesLookup(&Tseries[m_pumpTSIdx], date, FALSE);
    else
        return 0.0;
}


double ICAP::getPumpedVolume(double date)
{
    double Q = getPumpedRate(date);

    return Q * UCF(FLOW) * RouteStep;
}


double ICAP::computePipeStorage()
{
	double volume = 0.0;

	for (int i = 0; i < Nobjects[LINK]; i++)
	{
		volume += Link[i].newVolume;
	}

	return volume;
}


double ICAP::computePondedPipeStorage(double h)
{
    //TODO: fix this function so that it works for Mainstream situations
    // as well (e.g. no adverse slope to reservoir but reservoir is on a
    // branch and one of the branches slopes down lower than the res.

    InitializeZeroFlows();

	// Set zero depth in the reservoir to compute ponded volume.
    Node[m_sinkNodeIdx].newDepth = h;

    // Route the small depth through the pipe network.
    steadyRoute(m_sinkNodeIdx, true); // true ==> ponded

	return computePipeStorage();
}


double ICAP::computePondedPipeStorage()
{
    return computePondedPipeStorage(Node[m_sinkNodeIdx].invertElev);
}


void ICAP::SaveTotalVolumeCurve(char* file)
{
	//X = volume, Y = depth
	double minX, minY, maxX, maxY;
	table_getFirstEntry(&m_totalVolumeCurve, &minX, &minY);
	table_getLastEntry(&m_totalVolumeCurve, &maxX, &maxY);

	double incs = 100.0;
    double inc = (maxY - minY) / incs;

	FILE* tvcFile = fopen(file, "w");

	fprintf(tvcFile, "STAGE-STORAGE CURVE\nDEPTH\tVOLUME\n");

	double curY = minY;

	while (curY <= (maxY + ICAP_ZERO))
	{
		double curX = table_inverseLookup(&m_totalVolumeCurve, curY);
		fprintf(tvcFile, "%f\t%f\n", curY, curX);
		curY += inc;
	}

	fclose(tvcFile);
}


double ICAP::computeTotalVolumeCurve(TTable& curve)
{
    //TODO: fix this function so that it works for Mainstream situations
    // as well (e.g. no adverse slope to reservoir but reservoir is on a
    // branch and one of the branches slopes down lower than the res.

    //double resInvert = Node[m_sinkNodeIdx].invertElev;
    //double pipeCrown = Link[m_sinkLinkIdx].xsect.yFull + Node[m_sinkNodeIdx].invertElev;
    double maxHeight = Node[m_sinkNodeIdx].invertElev + Node[m_sinkNodeIdx].fullDepth;

    double maxVal = maxHeight;
    double minVal = 1000000.0;
    double incs = 100.0;
    double curStorage = 0.0;

    for (int i = 0; i < Nobjects[NODE]; i++)
    {
        if (Node[i].invertElev < minVal)
            minVal = Node[i].invertElev;
    }

    // 50 increments
	double inc = (maxVal - minVal) / incs;
    double curElev = minVal;

    InitializeZeroFlows();

    while (curElev <= (maxVal + ICAP_ZERO))
    {
		printf("%f\n", curElev);
	    // Set a very small depth to compute ponded volume.
	    Node[m_sinkNodeIdx].newDepth = curElev;

        // Route the small depth through the pipe network.
        steadyRoute(m_sinkNodeIdx, true); // true ==> ponded

        curStorage = computePipeStorage() +
            node_getVolume(m_sinkNodeIdx, curElev - Node[m_sinkNodeIdx].invertElev);

        table_addEntry(&curve, curStorage, curElev);

        curElev += inc;
    }

    if (table_validate(&curve) == FALSE)
        return curStorage; // return the largest storage capacity.
    else
        return -1.0;
}


double ICAP::getSystemHead(double vol)
{
    return table_lookup(&m_totalVolumeCurve, vol);
}


