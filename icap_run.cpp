#define _CRT_SECURE_NO_DEPRECATE

#include "icap.h"
#include "util.h"
#include "exception.h"
#include "icap_error.h"
#include "debug.h"
#include "math.h"


extern "C" TRoutingTotals   FlowTotals; // defined in MASSBAL.C
extern "C" TRoutingTotals   StepFlowTotals;  // defined in massbal.c
extern "C" TNodeStats*      NodeStats; // defined in stats.c


bool ICAP::End()
{
    bool result = true;

    FlowTotals.pumpedVol = V_P;

    int swmmRes = swmm_end(); // returns 1 in case of error, 0 if success
    result = (swmmRes ? false : true);
    
    if (!result)
    {
        WriteErrorMsg("Unable to end Engine.");
        ErrorCode = ERR_SYSTEM;
        return false;//cleanup();
    }

    return result;
}


bool ICAP::Close()
{
    bool result = true;

    // Clean up all of the SWMM objects.
    int swmmRes = swmm_close(); // returns 1 in case of error, 0 if success
    result = (swmmRes ? false : true);

    if (! result)
    {
        WriteErrorMsg("Unable to close Engine.");
        ErrorCode = ERR_SYSTEM;
        return false;//cleanup();
    }

    return result;
}


bool ICAP::Step(double* elapsedTime)
{
    bool result = true;

    try
    {

	m_counter++;

    if (NewRoutingTime > TotalDuration)
    {
        *elapsedTime = 0.0;
        return true;
    }

    // Lowercase variable names mean specific to the current timestep.
    
    double y_r = 0.0; // y_r = depth in reservoir
	bool toContinue = true;

    double curStep = NewRoutingTime;
    double currentDate = getDateTime(curStep);

    // Format the time at the current timestep.
    char datetimeBuf[30];
    TSDateTimeStr(currentDate, datetimeBuf);

#ifdef ICAP_DEBUGGING
    dprintf("%s %f ", datetimeBuf, currentDate);
#endif


	/////////////////////////////////////////////////////////////////
	// SWMM MASS-BALANCE ERROR CALCS

	// Update mass-balance error calculation over 1/2 the step.
	massbal_updateRoutingTotals(RouteStep / 2.0);

	// Initialize the totals for the current time step.
    massbal_initTimeStepTotals();


	/////////////////////////////////////////////////////////////////
	// INFLOWS TO THE SYSTEM

	// Determine the inflow to the input nodes.
    if (!m_rtMode)
    {
        initializeFlows(currentDate);
    }

	// Propogate the flows down from the input nodes to the reservoir.
    propogateFlows();


	/////////////////////////////////////////////////////////////////
	// TOTAL INFLOW

    // First get the flow at the reservoir for this timestep.
    double flow = getFlowAtNode(m_sinkNodeIdx);

    // Get the inflow volume at the current step.
    // UCF(FLOW) converts from the user-specified units to CFS
	double v_it = flow * UCF(FLOW) * RouteStep; // v_it <==> inputVol

    y_r = Node[m_sinkNodeIdx].newDepth;

    /////////////////////////////////////////////////////////////////
    // REGIME DETERMINATION

    // Compute the regime that we're in.  Ponded regime means that the
    // flow is low enough to be considered negligible for flow routing
    // purposes.  Steady State regime means that flows are high enough
    // to perform a steady-state routing.
    
    double v_pt = computePumpedVolume(flow, currentDate); // v_pt <==> pumpVol

    if (IS_ZERO(flow))
    {
        if (IS_ZERO(y_r) && IS_ZERO(V_Pond))
            m_regime = Regime_Empty;
        else if (IS_ZERO(y_r))
            m_regime = Regime_PondedDraining;
        else
            m_regime = Regime_Draining;
    }
    else if (IS_NOT_ZERO(PumpingThreshold) && flow <= PumpingThreshold)
    {
        if (y_r > 0.0)
            m_regime = Regime_Draining;
        else if (IS_NOT_ZERO(V_Pond))
            m_regime = Regime_PondedDraining;
        else
            m_regime = Regime_PondedFilling;
    }
    else
    {
        if (IS_NOT_ZERO(v_pt) && m_pumpTSIdx >= 0)
        {
            if (IS_ZERO(y_r))
                m_regime = Regime_PondedDraining;
            else
                m_regime = Regime_Draining;
        }
        else if (IS_ZERO(y_r))
        {
            if ((V_Pond + v_it) > V_PondMax) // this is equivalent to (V_I > v_t + V_P), we're spilling over into the reservoir
                m_regime = Regime_SteadyState;
            else
                m_regime = Regime_PondedFilling;
        }
        else
            m_regime = Regime_SteadyState;
    }

#ifdef ICAP_DEBUGGING
	//dprintf("%d %f\n", m_regime, PumpingThreshold);
#endif

	// The pumped-out volume is reset to zero if the regime is not draining or
    // ponded draining.  It's also set to the ponded volume if the pumped
    // volume is > ponded volume.
    if ((m_regime == Regime_Draining && v_pt > V_Pond) || (m_regime == Regime_PondedDraining && v_pt > V_Pond))
    {
        v_pt = V_Pond;
    }
    else if (m_regime != Regime_Draining && m_regime != Regime_PondedDraining)
    {
        v_pt = 0.0;
    }

    // v_ot is the volume that has overflowed the reservoir at this time.
    double v_ot = ((V_I + v_it) - (V_P + v_pt)) - V_SysMax;

#ifdef ICAP_DEBUGGING
	dprintf("%f %f %f %f ", flow, v_it, v_ot, v_pt);
#endif

    // If v_ot is positive, then we've overflowed the reservoir.  Add v_ot
    // to V_Ov and update V_I if necessary.
    if (v_ot > 0.0)
    {
        V_Ov += v_ot;
        V_I += (v_it - v_ot);
    }
	// Otherwise, compute the total inflow volume up to this point.
    else
        V_I += v_it;

#ifdef ICAP_DEBUGGING
	dprintf("%f %f\n", V_I, V_Ov);
#endif

    // Update the total pumping counter.
    V_P += v_pt;
	
	
	/////////////////////////////////////////////////////////////////
	// ROUTING

    if (m_regime == Regime_Empty)
    {
        InitializeZeroDepths();
    }
    else if (m_regime == Regime_PondedDraining ||
             m_regime == Regime_Draining)
    {
        toContinue = Step_Draining();
    }
    else if (m_regime == Regime_PondedFilling)
    {
        toContinue = Step_PondedFilling();

        if (! toContinue)
        {
            char msg[1000];
            sprintf(msg, "Routing failed at time %s (error in mass balance, expected V_I > V_P [ponded/filling regime])", datetimeBuf);
            setError(1, msg);
            WriteErrorMsg(msg);
            ErrorCode = ERR_SYSTEM;
            return false;
        }
    }
    else if (m_regime == Regime_SteadyState)
    {
        toContinue = Step_SteadyState();

        if (! toContinue)
        {
            char msg[1000];
            sprintf(msg, "Routing failed at time %s (routing error: %s)", datetimeBuf, m_errorStr.c_str());
            setError(1, msg);
            WriteErrorMsg(msg);
            ErrorCode = ERR_SYSTEM;
            return false;
        }
    }


	/////////////////////////////////////////////////////////////////
	// SWMM MASS-BALANCE ERROR CALCS

	// Update mass-balance error calculation over 1/2 the step.
	massbal_updateRoutingTotals(RouteStep / 2.0);


	/////////////////////////////////////////////////////////////////
	// STATISTICS AND REPORTING

    // Flooding is defined as the water height (depth + invert) exceeding
    // the global overflow value.
    bool hasOverflowed = updateOverflows();

    // Update the statistics for the report file.
    stats_updateFlowStats(RouteStep, currentDate, 1, TRUE);

    if ( NewRoutingTime >= ReportTime )
    {
        output_saveResults(ReportTime);
        ReportTime = ReportTime + (double)(1000 * ReportStep);
    }
    //// Output the results for the current time.  We ignore the reporting time
    //// field in the SWMM options and just output based on the routing step.
    //output_saveResults(curStep);

    // Reset the error code so that an ignorable error doesn't stop the
    // run next time.
    m_errorCode = 0;

#ifdef ICAP_DEBUGGING
    // Write the routing results to the output file.
    //OutputTimeStep(datetimeBuf, m_debugFile);
#endif

    curStep += RouteStep * 1000.0;
    NewRoutingTime = curStep;

    *elapsedTime = curStep / MSECperDAY;

    }
    catch(...)
    {
        result = false;
        ErrorCode = ERR_SYSTEM;
        WriteErrorMsg("Exception in step");
    }

    return result;
}


bool ICAP::Step_PondedFilling()
{
    bool toContinue = true;

    V_Pond = V_I - V_P;

    // Compute the new water depth after filling it up a little.
    if (IS_NOT_ZERO(V_Pond) && V_Pond > 0.0)
    {
        double elev = getSystemHead(V_I - V_P);
        Node[m_sinkNodeIdx].newDepth = elev;
        Node[m_sinkNodeIdx].newVolume = node_getVolume(m_sinkNodeIdx, elev - Node[m_sinkNodeIdx].invertElev);
        toContinue = steadyRoute(m_sinkNodeIdx, true);
    }
    else
        toContinue = false;

    return toContinue;
}


bool ICAP::Step_Draining()
{
    bool toContinue = true;

    double elev = getSystemHead(V_I - V_P);
    Node[m_sinkNodeIdx].newDepth = elev;
    toContinue = steadyRoute(m_sinkNodeIdx, true);
    
    double nodeDepth = elev - Node[m_sinkNodeIdx].invertElev;

    V_Pond = V_I - V_P;

    if (nodeDepth <= 0.0) // now we're empty
    {
        Node[m_sinkNodeIdx].newVolume = 0.0;
        Node[m_sinkNodeIdx].newDepth = 0.0;
    }
    else
    {
        Node[m_sinkNodeIdx].newVolume = node_getVolume(m_sinkNodeIdx, elev - Node[m_sinkNodeIdx].invertElev);
        Node[m_sinkNodeIdx].newDepth = nodeDepth;
        //V_Pond = V_PondMax;
    }

    if (IS_ZERO(V_Pond) || V_Pond <= 0.0)
    {
        V_Pond = 0.0;
        InitializeZeroDepths();
    }

    return toContinue;
}


bool ICAP::Step_SteadyState()
{
    double v_r = 0.0;
    double v_t = 0.0;

    m_errorCode = 0;

    // Set the downstream boundary condition to be the level pool depth
    // given the current system volume.
    double elev = getSystemHead(V_I - V_P);
    Node[m_sinkNodeIdx].newDepth = elev - Node[m_sinkNodeIdx].invertElev;
    if (Node[m_sinkNodeIdx].newDepth < 0.0)
        Node[m_sinkNodeIdx].newDepth = 0.0;
    Node[m_sinkNodeIdx].newVolume = node_getVolume(m_sinkNodeIdx, Node[m_sinkNodeIdx].newDepth);
    
    // Using HPG's determine the water surface profile through the
    // pipe system.
    if (! steadyRoute(m_sinkNodeIdx, false))
        return false;

    // Reset ponded volume for future calculations.
    V_Pond = V_I - V_P;

    return true;
}


bool ICAP::updateOverflows()
{
    bool result = false;
    for (int i = 0; i < Nobjects[NODE]; i++)
    {
        if (Node[i].canFlood &&
            Node[i].newDepth > Node[i].fullDepth)
        {
            Node[i].overflow = Node[i].newDepth - Node[i].fullDepth;
            NodeStats[i].timeFlooded += RouteStep;

            if (! m_overflow.IsInEvent(i))
                StepFlowTotals.floodingNumNodes += 1.0;

            result = true;
        }
        else
        {
            Node[i].overflow = 0.0;
            m_overflow.Reset(i);
        }
    }

    if (result)
        StepFlowTotals.floodingNumNodes += 1.0;

    return result;
}


bool ICAP::RunSimulation(int sinkNodeIdx)
{
    using namespace std;
    double curStep = 0.0;

    // Compute the initial volume and water depth of the sink node.
    double volume = node_getVolume(sinkNodeIdx, Node[sinkNodeIdx].initDepth);
    Node[sinkNodeIdx].newDepth = Node[sinkNodeIdx].initDepth;

    // Now iterate over each timestep and route the system for that step.
    bool toContinue = true;
    int iterCount = 0;
    while (curStep <= TotalDuration)
    {
        if (! Step(&curStep) )
        {
            toContinue = false;
            break;
        }
        iterCount++;
    }

    return toContinue;
}


void ICAP::SetRealTimeStatus(bool enabled)
{
    m_rtMode = enabled;
}
