#define _CRT_SECURE_NO_DEPRECATE

#include "icap.h"
#include "util.h"
#include "exception.h"
#include "icap_error.h"
#include "debug.h"
#include "math.h"


#if defined(SWMM_GEOMETRY)
extern "C" TRoutingTotals   FlowTotals; // defined in MASSBAL.C
extern "C" TRoutingTotals   StepFlowTotals;  // defined in massbal.c
extern "C" TNodeStats*      NodeStats; // defined in stats.c
#endif


bool ICAP::End()
{
    bool result = true;

    FlowTotals.pumpedVol = V_P;

#if defined(SWMM_GEOMETRY)
    int swmmRes = swmm_end(); // returns 1 in case of error, 0 if success
    result = (swmmRes ? false : true);

    if (!result)
    {
        WriteErrorMsg("Unable to end Engine.");
        ErrorCode = ERROR_SYSTEM;
        return false;//cleanup();
    }
#endif

    return result;
}


bool ICAP::Close()
{
    bool result = true;

#if defined(SWMM_GEOMETRY)
    // Clean up all of the SWMM objects.
    int swmmRes = swmm_close(); // returns 1 in case of error, 0 if success
    result = (swmmRes ? false : true);

    if (! result)
    {
        WriteErrorMsg("Unable to close Engine.");
        ErrorCode = ERROR_SYSTEM;
        return false;//cleanup();
    }
#endif

    return result;
}

//bool ICAP::StepMatrix(double* elapsedTime)
//{
//    return Step(elapsedTime, true);
//    bool result = true;
//
//    try
//    {
//
//	m_counter++;
//
//    if (NewRoutingTime > TotalDuration)
//    {
//        *elapsedTime = 0.0;
//        return true;
//    }
//
//    // Lowercase variable names mean specific to the current timestep.
//    
//    double y_r = 0.0; // y_r = depth in reservoir
//	bool toContinue = true;
//
//    double curStep = NewRoutingTime;
//    double currentDate = getDateTime(curStep);
//
//    // Format the time at the current timestep.
//    char datetimeBuf[30];
//    TSDateTimeStr(currentDate, datetimeBuf);
//
//#ifdef ICAP_DEBUGGING
//    dprintf("%s %f ", datetimeBuf, currentDate);
//#endif
//
//
//	/////////////////////////////////////////////////////////////////
//	// SWMM MASS-BALANCE ERROR CALCS
//
//	// Update mass-balance error calculation over 1/2 the step.
//	//massbal_updateRoutingTotals(RouteStep / 2.0);
//
//	// Initialize the totals for the current time step.
//    //massbal_initTimeStepTotals();
//
//
//	/////////////////////////////////////////////////////////////////
//	// INFLOWS TO THE SYSTEM
//
//	// Determine the inflow to the input nodes.
//    if (!m_rtMode)
//    {
//        initializeFlows(currentDate);
//    }
//
//	propogateFlows();
//
//
//
//
//	/////////////////////////////////////////////////////////////////
//	// SWMM MASS-BALANCE ERROR CALCS
//
//	// Update mass-balance error calculation over 1/2 the step.
//	//massbal_updateRoutingTotals(RouteStep / 2.0);
//
//
//	/////////////////////////////////////////////////////////////////
//	// STATISTICS AND REPORTING
//
//    // Flooding is defined as the water height (depth + invert) exceeding
//    // the global overflow value.
//    bool hasOverflowed = updateOverflows();
//
//    // Update the statistics for the report file.
//    stats_updateFlowStats(RouteStep, currentDate, 1, TRUE);
//
//    if ( NewRoutingTime >= ReportTime )
//    {
//        output_saveResults(ReportTime);
//        ReportTime = ReportTime + (double)(1000 * ReportStep);
//    }
//    //// Output the results for the current time.  We ignore the reporting time
//    //// field in the SWMM options and just output based on the routing step.
//    //output_saveResults(curStep);
//
//    // Reset the error code so that an ignorable error doesn't stop the
//    // run next time.
//    m_errorCode = 0;
//
//#ifdef ICAP_DEBUGGING
//    // Write the routing results to the output file.
//    //OutputTimeStep(datetimeBuf, m_debugFile);
//#endif
//
//    curStep += RouteStep * 1000.0;
//    NewRoutingTime = curStep;
//
//    *elapsedTime = curStep / MSECperDAY;
//
//    }
//    catch(...)
//    {
//        result = false;
//        ErrorCode = ERROR_SYSTEM;
//        WriteErrorMsg("Exception in step");
//    }
//
//	return result;
//}

bool ICAP::Step(double* elapsedTime, bool useMatrix)
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

#if defined(SWMM_GEOMETRY)
	// Update mass-balance error calculation over 1/2 the step.
	massbal_updateRoutingTotals(RouteStep / 2.0);

	// Initialize the totals for the current time step.
    massbal_initTimeStepTotals();
#endif

	/////////////////////////////////////////////////////////////////
	// INFLOWS TO THE SYSTEM

	// Determine the inflow to the input nodes.
    if (!m_rtMode)
    {
        m_model->resetTimestep(currentDate);
    }

	// Propogate the flows down from the input nodes to the reservoir.
    m_model->startTimestep(currentDate);


	/////////////////////////////////////////////////////////////////
	// TOTAL INFLOW

    // First get the flow at the reservoir for this timestep.
    double flow = getFlowAtNode(m_sinkNodeIdx);

    // Get the inflow volume at the current step.
    // UCF(FLOW) converts from the user-specified units to CFS
	double v_it = flow * UCF(FLOW) * RouteStep; // v_it <==> inputVol

    y_r = GNODE_DEPTH(m_sinkNodeIdx);

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
            ErrorCode = ERROR_SYSTEM;
            return false;
        }
    }
    else if (m_regime == Regime_SteadyState)
    {
        toContinue = Step_SteadyState(useMatrix);

        if (! toContinue)
        {
            char msg[1000];
            sprintf(msg, "Routing failed at time %s (routing error: %s)", datetimeBuf, m_errorStr.c_str());
            setError(1, msg);
            WriteErrorMsg(msg);
            ErrorCode = ERROR_SYSTEM;
            return false;
        }
    }


	/////////////////////////////////////////////////////////////////
	// SWMM MASS-BALANCE ERROR CALCS
    
#if defined(SWMM_GEOMETRY)
	// Update mass-balance error calculation over 1/2 the step.
	massbal_updateRoutingTotals(RouteStep / 2.0);
#endif

	/////////////////////////////////////////////////////////////////
	// STATISTICS AND REPORTING

    // Flooding is defined as the water height (depth + invert) exceeding
    // the global overflow value.
    bool hasOverflowed = updateOverflows();

    // Update the statistics for the report file.
#if defined(SWMM_GEOMETRY)
    stats_updateFlowStats(RouteStep, currentDate, 1, TRUE);
#else
#error TODO
#endif

    if ( NewRoutingTime >= ReportTime )
    {
#if defined(SWMM_GEOMETRY)
        output_saveResults(ReportTime);
#else
#error TODO
#endif
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
        ErrorCode = ERROR_SYSTEM;
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
        GNODE_DEPTH(m_sinkNodeIdx) = elev;
        GNODE_VOLUME(m_sinkNodeIdx) = GCURVE_VOLUME(m_sinkNodeIdx, elev - GNODE_INVERT(m_sinkNodeIdx));
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
    GNODE_DEPTH(m_sinkNodeIdx) = elev;
    toContinue = steadyRoute(m_sinkNodeIdx, true);
    
    double nodeDepth = elev - GNODE_INVERT(m_sinkNodeIdx);

    V_Pond = V_I - V_P;

    if (nodeDepth <= 0.0) // now we're empty
    {
        GNODE_VOLUME(m_sinkNodeIdx) = 0.0;
        GNODE_DEPTH(m_sinkNodeIdx) = 0.0;
    }
    else
    {
        GNODE_VOLUME(m_sinkNodeIdx) = GCURVE_VOLUME(m_sinkNodeIdx, elev - GNODE_INVERT(m_sinkNodeIdx));
        GNODE_DEPTH(m_sinkNodeIdx) = nodeDepth;
        //V_Pond = V_PondMax;
    }

    if (IS_ZERO(V_Pond) || V_Pond <= 0.0)
    {
        V_Pond = 0.0;
        InitializeZeroDepths();
    }

    return toContinue;
}


bool ICAP::Step_SteadyState(bool useMatrix)
{
    double v_r = 0.0;
    double v_t = 0.0;

    m_errorCode = 0;

    // Set the downstream boundary condition to be the level pool depth
    // given the current system volume.
    double elev = getSystemHead(V_I - V_P);
    GNODE_DEPTH(m_sinkNodeIdx) = elev - GNODE_INVERT(m_sinkNodeIdx);
    if (GNODE_DEPTH(m_sinkNodeIdx) < 0.0)
        GNODE_DEPTH(m_sinkNodeIdx) = 0.0;
    GNODE_VOLUME(m_sinkNodeIdx) = GCURVE_VOLUME(m_sinkNodeIdx, GNODE_DEPTH(m_sinkNodeIdx));
    
    if (useMatrix)
    {
        if (!m_isFirstMatrixIteration)
        {
            setInitialMatrixDepthGuess();
            m_isFirstMatrixIteration = true;
        }

        if (!iterateMatrix())
        {
            return false;
        }
    }
    else
    {
        // Using HPG's determine the water surface profile through the
        // pipe system.
        if (! steadyRoute(m_sinkNodeIdx, false))
            return false;
    }

    // Reset ponded volume for future calculations.
    V_Pond = V_I - V_P;

    return true;
}


void ICAP::setInitialMatrixDepthGuess()
{
    // Loop over every node and set an initial guess.
    for (int i = 0; i < GNODE_COUNT; i++)
    {
        if (i == m_sinkNodeIdx)
            continue;

        ICAPNode* node = m_network.FindNode(i);
        GNODE_DEPTH(i) = 100000;

        for (int j = 0; j < node->linkIdx.size(); j++)
        {
            double diam2 = GLINK_MAXDEPTH(node->linkIdx.at(j)) / 2;
            if (diam2 < GNODE_DEPTH(i))
            {
                GNODE_DEPTH(i) = diam2;
            }
        }
    }
}


bool ICAP::iterateMatrix()
{
    bool result = true;

    float maxError = 1;

    Eigen::VectorXf curQ(GLINK_COUNT);
    Eigen::VectorXf curH(GNODE_COUNT);

    // Copy values from previous time step into temp vars.
    for (int i = 0; i < GLINK_COUNT; i++)
        curQ(i) = GLINK_FLOW(i);
    for (int i = 0; i < GNODE_COUNT; i++)
        curH(i) = GNODE_DEPTH(i);

    // Perform operations until convergence or number of iterations exceeds a given threshold.
    int numIter = 0;
    int maxIter = 1000;
    do
    {
        if (!setHfAndDE(curQ, curH))
        {
            return false;
        }

        Eigen::VectorXf delta = m_matrixLhs.inverse() * m_matrixRhs;
        maxError = fabs(delta.maxCoeff());

        for (int i = 0; i < GLINK_COUNT; i++)
            curQ(i) += delta(i);;
        for (int i = 0; i < GNODE_COUNT; i++)
            curH(i) += delta(i + GLINK_COUNT);
    }
    while (maxError > 1e-4 && numIter++ < maxIter);

    // If number of iterations exceeds a given threshold, then return false (error in simulation), 
    // otherwise copy the values from the temp vars into the permanent vars.
    if (numIter >= maxIter)
    {
        return false;
    }
    else
    {
        for (int i = 0; i < GLINK_COUNT; i++)
            GLINK_FLOW(i) = curQ(i);
        for (int i = 0; i < GNODE_COUNT; i++)
            GNODE_DEPTH(i) = curH(i);
    }

    return result;
}


bool ICAP::setHfAndDE(Eigen::VectorXf& curQ, Eigen::VectorXf& curH)
{
    bool result = true;

    m_matrixRhs.setZero();

    // Loop over every node and set an initial guess.
    for (int i = 0; i < GLINK_COUNT; i++)
    {
        double hf = 0;
        if (!m_hpgList.GetHf(i, GNODE_DEPTH(GLINK_DSNODE(i)), GLINK_FLOW(i), hf))
        {
            return false;
        }

        m_matrixLhs(i, i) = hf;

        m_matrixRhs(i) = hf + GNODE_DEPTH(GLINK_DSNODE(i)) - GNODE_DEPTH(GLINK_USNODE(i));

        m_matrixRhs(GLINK_COUNT + GLINK_DSNODE(i)) += GLINK_FLOW(i);
        m_matrixRhs(GLINK_COUNT + GLINK_USNODE(i)) -= GLINK_FLOW(i);
    }

    double totalLatFlow = 0;
    for (int i = 0; i < GNODE_COUNT; i++)
    {
        m_matrixRhs(GLINK_COUNT + i) += GNODE_LATFLOW(i);
        totalLatFlow += GNODE_LATFLOW(i);
    }

    m_matrixRhs(GLINK_COUNT + m_sinkNodeIdx) -= totalLatFlow;

    for (int i = 0; i < m_matrixRhs.size(); i++)
    {
        m_matrixRhs(i) = - m_matrixRhs(i);
    }

    return result;
}


bool ICAP::updateOverflows()
{
    bool result = false;
    for (int i = 0; i < GNODE_COUNT; i++)
    {
        if (GNODE_CANFLOOD(i) &&
            GNODE_DEPTH(i) > GNODE_MAXDEPTH(i))
        {
            GNODE_OVERFLOW(i) = GNODE_DEPTH(i) - GNODE_MAXDEPTH(i);
            GSYS_NODESTATS_FLOODING(i) += RouteStep;

            if (! m_overflow.IsInEvent(i))
                GSYS_NODESTATS_NUMNODESFLOODING += 1.0;

            result = true;
        }
        else
        {
            GNODE_OVERFLOW(i) = 0.0;
            m_overflow.Reset(i);
        }
    }

    if (result)
        GSYS_NODESTATS_NUMNODESFLOODING += 1.0;

    return result;
}


bool ICAP::RunSimulation(int sinkNodeIdx)
{
    using namespace std;
    double curStep = 0.0;

    // Compute the initial volume and water depth of the sink node.
    double volume = GCURVE_VOLUME(sinkNodeIdx, GNODE_INITDEPTH(sinkNodeIdx));
    GNODE_DEPTH(sinkNodeIdx) = GNODE_INITDEPTH(sinkNodeIdx);

    // Now iterate over each timestep and route the system for that step.
    bool toContinue = true;
    int iterCount = 0;
    while (curStep <= TotalDuration)
    {
        if (! Step(&curStep, false) )
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
