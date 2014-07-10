#define _USE_MATH_DEFINES

#include "icap.h"
#include "routing.h"
#include <cmath>
#include <vector>


#if defined(SWMM_GEOMETRY)
extern "C" double* NodeInflow;             // defined in MASSBAL.C
#endif


void ICAP::InitializeZeroFlows()
{
    m_model->resetDepths();
}


void ICAP::InitializeZeroDepths()
{
    m_model->resetDepths();
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
        flow += GLINK_FLOW(linkIdx);
    }

	flow += GNODE_LATFLOW(idx);

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
        return GTABLE_TSLOOKUP(m_pumpTSIdx, date);
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

	for (int i = 0; i < GLINK_COUNT; i++)
	{
		volume += GLINK_VOLUME(i);
	}

	return volume;
}


double ICAP::computePondedPipeStorage(double h)
{
    //TODO: fix this function so that it works for Mainstream situations
    // as well (e.g. no adverse slope to reservoir but reservoir is on a
    // branch and one of the branches slopes down lower than the res.

    m_model->resetTimestep(0);

	// Set zero depth in the reservoir to compute ponded volume.
    GNODE_DEPTH(m_sinkNodeIdx) = h;

    // Route the small depth through the pipe network.
    steadyRoute(m_sinkNodeIdx, true); // true ==> ponded

	return computePipeStorage();
}


double ICAP::computePondedPipeStorage()
{
    return computePondedPipeStorage(GNODE_INVERT(m_sinkNodeIdx));
}


void ICAP::SaveTotalVolumeCurve(char* file)
{
#if defined(SWMM_GEOMETRY)
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
#else
#error TODO
#endif
}


double ICAP::computeTotalVolumeCurve(TTable& curve)
{
    //TODO: fix this function so that it works for Mainstream situations
    // as well (e.g. no adverse slope to reservoir but reservoir is on a
    // branch and one of the branches slopes down lower than the res.

    //double resInvert = GNODE_INVERT(m_sinkNodeIdx);
    //double pipeCrown = GLINK_MAXDEPTH(m_sinkLinkIdx) + GNODE_INVERT(m_sinkNodeIdx);
    double maxHeight = GNODE_INVERT(m_sinkNodeIdx) + GNODE_MAXDEPTH(m_sinkNodeIdx);

    double maxVal = maxHeight;
    double minVal = 1000000.0;
    double incs = 100.0;
    double curStorage = 0.0;

    for (int i = 0; i < GNODE_COUNT; i++)
    {
        if (GNODE_INVERT(i) < minVal)
            minVal = GNODE_INVERT(i);
    }

    // 50 increments
	double inc = (maxVal - minVal) / incs;
    double curElev = minVal;

    m_model->resetTimestep(0);

    while (curElev <= (maxVal + ICAP_ZERO))
    {
		//printf("%f\n", curElev);
	    // Set a very small depth to compute ponded volume.
	    GNODE_DEPTH(m_sinkNodeIdx) = curElev;

        // Route the small depth through the pipe network.
        steadyRoute(m_sinkNodeIdx, true); // true ==> ponded

        curStorage = computePipeStorage() +
            GCURVE_VOLUME(m_sinkNodeIdx, curElev - GNODE_INVERT(m_sinkNodeIdx)) + 1e-6;  // + 1e-6 since we don't want it to be zero

        GTABLE_ADDENTRY(curve, curStorage, curElev);

        curElev += inc;
    }

    if (GTABLE_VALIDATE(curve) == FALSE)
        return curStorage; // return the largest storage capacity.
    else
        return -1.0;
}


double ICAP::getSystemHead(double vol)
{
    return GTABLE_LOOKUP(m_totalVolumeCurve, vol);
}


