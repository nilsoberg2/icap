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

#define _CRT_SECURE_NO_DEPRECATE

#include "../model/units.h"
#include "../util/parse.h"
#include "../util/math.h"

#include "icap.h"
#include "exception.h"
#include "logging.h"
#include "Stopwatch.h"


bool ICAP::End()
{
    bool result = true;

    m_geometry->updateSystemStatistic(statvariables::PumpedVolume, V_P);

    m_results.complete();
    m_report.complete();

    return result;
}


bool ICAP::Close()
{
    bool result = true;

    return result;
}


bool ICAP::Step(double* elapsedTime, bool useMatrix)
{
    return Step(elapsedTime, m_routeStep, useMatrix);
}


bool ICAP::Step(double* elapsedTime, double routeStep, bool useMatrix)
{
    Stopwatch swatch;
    swatch.set_mode(StopwatchMode::CPU_TIME);

    swatch.start("Step");

    bool result = true;

	m_counter++;
    
    if (!m_realTimeFlows && m_newRoutingTime > m_totalDuration)
    {
        *elapsedTime = 0.0;
        return true;
    }

    // Lowercase variable names mean specific to the current timestep.
    
    double y_r = 0.0; // y_r = depth in reservoir
	bool toContinue = true;
    
    double curStep = m_newRoutingTime;
    DateTime currentDate = m_geometry->getCurrentDateTime();

    // Format the time at the current timestep.
    std::string datetimeBuf = currentDate.toString();
    BOOST_LOG_SEV(m_log, loglevel::info) << datetimeBuf;
    
    //try
    //{

	/////////////////////////////////////////////////////////////////
	// INFLOWS TO THE SYSTEM

	// Determine the inflow to the input nodes.
    if (!m_realTimeFlows)
    {
        m_geometry->resetTimestep();
    }

	// Propogate the flows down from the input nodes to the reservoir.
    m_geometry->startTimestep(currentDate);


	/////////////////////////////////////////////////////////////////
	// TOTAL INFLOW

    // First get the flow at the reservoir for this timestep.
    double flow = getFlowAtNode(m_sinkNodeIdx);

    // Get the inflow volume at the current step.
    // UCF(FLOW) converts from the user-specified units to CFS
	double v_it = flow * UCF->flow() * routeStep; // v_it <==> inputVol

    y_r = m_geometry->getNodeVariable(m_sinkNodeIdx, variables::NodeDepth);

    /////////////////////////////////////////////////////////////////
    // REGIME DETERMINATION

    // Compute the regime that we're in.  Ponded regime means that the
    // flow is low enough to be considered negligible for flow routing
    // purposes.  Steady State regime means that flows are high enough
    // to perform a steady-state routing.
    
    double v_pt = m_pumping.computePumpedVolume(flow, routeStep, currentDate); // v_pt <==> pumpVol

    if (isZero(flow))
    {
        if (isZero(y_r) && isZero(V_Pond))
            m_regime = Regime_Empty;
        else if (isZero(y_r))
            m_regime = Regime_PondedDraining;
        else
            m_regime = Regime_Draining;
    }
    else if (m_pumping.hasThreshold() && flow <= m_pumping.getThreshold())
    {
        if (y_r > 0.0)
            m_regime = Regime_Draining;
        else if (!isZero(V_Pond))
            m_regime = Regime_PondedDraining;
        else
            m_regime = Regime_PondedFilling;
    }
    else
    {
        if (!isZero(v_pt) && m_pumping.hasPumpingTimeseries())
        {
            if (isZero(y_r))
                m_regime = Regime_PondedDraining;
            else
                m_regime = Regime_Draining;
        }
        else if (isZero(y_r))
        {
            if ((V_Pond + v_it) > V_PondMax) // this is equivalent to (V_I > v_t + V_P), we're spilling over into the reservoir
                m_regime = Regime_SteadyState;
            else
                m_regime = Regime_PondedFilling;
        }
        else
            m_regime = Regime_SteadyState;
    }

    BOOST_LOG_SEV(m_log, loglevel::debug) << m_regime;

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

    BOOST_LOG_SEV(m_log, loglevel::debug) << "qRes=" << flow << " v_it=" << v_it << " v_ot=" << v_ot << " v_pt=" << v_pt;

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

    // Update the total pumping counter.
    V_P += v_pt;

    BOOST_LOG_SEV(m_log, loglevel::debug) << "V_I=" << V_I << " V_Ov=" << V_Ov << " V_P=" << V_P;
	
	
	/////////////////////////////////////////////////////////////////
	// ROUTING

    // If a manual downstream boundary has been specified, then we need to force a steady state
    // computation.
    if (m_realTimeDsHead)
    {
        BOOST_LOG_SEV(m_log, loglevel::debug) << "Regime: Steady/ForcedDsBoundary";
        toContinue = Step_SteadyState(useMatrix);

        if (! toContinue)
        {
            setErrorMessage("steady/forced failed");
            BOOST_LOG_SEV(m_log, loglevel::error) << "Routing failed at time=" << datetimeBuf << " (routing error: " << m_errorStr << ")";
            return false;
        }
    }
    else if (m_regime == Regime_Empty)
    {
        BOOST_LOG_SEV(m_log, loglevel::debug) << "Regime: Empty";
        InitializeZeroDepths();
    }
    else if (m_regime == Regime_PondedDraining ||
             m_regime == Regime_Draining)
    {
        BOOST_LOG_SEV(m_log, loglevel::debug) << "Regime: Draining";
        toContinue = Step_Draining();
    }
    else if (m_regime == Regime_PondedFilling)
    {
        BOOST_LOG_SEV(m_log, loglevel::debug) << "Regime: Filling";
        toContinue = Step_PondedFilling();

        if (! toContinue)
        {
            setErrorMessage("filling failed");
            BOOST_LOG_SEV(m_log, loglevel::error) << "Routing failed at time=" << datetimeBuf << ": (error in mass balance, expected V_I > V_P [ponded/filling regime])";
            return false;
        }
    }
    else if (m_regime == Regime_SteadyState)
    {
        BOOST_LOG_SEV(m_log, loglevel::debug) << "Regime: Steady";
        toContinue = Step_SteadyState(useMatrix);

        if (! toContinue)
        {
            setErrorMessage("steady failed");
            BOOST_LOG_SEV(m_log, loglevel::error) << "Routing failed at time=" << datetimeBuf << " (routing error: " << m_errorStr << ")";
            return false;
        }
    }


	/////////////////////////////////////////////////////////////////
	// STATISTICS AND REPORTING

    // Flooding is defined as the water height (depth + invert) exceeding
    // the global overflow value.
    bool hasOverflowed = updateOverflows(routeStep);

    updateTimestepStatistics();

    if ( m_newRoutingTime >= m_reportTime )
    {
        saveTimestepResults();
        m_reportTime = m_reportTime + (double)(1000 * m_reportStep);
    }
    //// Output the results for the current time.  We ignore the reporting time
    //// field in the SWMM options and just output based on the routing step.
    //output_saveResults(curStep);

    // Reset the error code so that an ignorable error doesn't stop the
    // run next time.
    m_errorCode = 0;

    // Write the routing results to the output file.
    //OutputTimeStep(datetimeBuf, m_debugFile);

    curStep += routeStep;
    m_newRoutingTime = curStep;
    
    *elapsedTime = curStep;

    //}
    //catch(...)
    //{
    //    result = false;
    //    BOOST_LOG_SEV(m_log, loglevel::error) << "Exception occured in timestep function at time=" << datetimeBuf;
    //    setErrorMessage("Exception occured in timestep function at time=" + datetimeBuf);
    //}

    swatch.stop("Step");

    BOOST_LOG_SEV(m_log, loglevel::debug) << "Step time: " << swatch.get_average_time("Step");

    return result;
}


bool ICAP::Step_PondedFilling()
{
    bool toContinue = true;

    V_Pond = V_I - V_P;
    std::shared_ptr<geometry::Node> node = m_geometry->getNode(m_sinkNodeIdx);

    // Compute the new water depth after filling it up a little.
    if (!isZero(V_Pond) && V_Pond > 0.0)
    {
        // Use elevation because of the ponded routing.
        var_type elev = getSystemHead(V_I - V_P);
        node->variable(variables::NodeDepth) = elev;
        node->variable(variables::NodeVolume) = node->lookupVolume(elev - node->getInvert());
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
    std::shared_ptr<geometry::Node> node = m_geometry->getNode(m_sinkNodeIdx);

    // Use elevation because of the ponded routing.
    node->variable(variables::NodeDepth) = elev;

    toContinue = steadyRoute(m_sinkNodeIdx, true);
    
    double nodeDepth = elev - node->getInvert();

    V_Pond = V_I - V_P;

    if (nodeDepth <= 0.0) // now we're empty
    {
        node->variable(variables::NodeVolume) = 0.0;
        node->variable(variables::NodeDepth) = 0.0;
    }
    else
    {
        node->variable(variables::NodeVolume) = node->lookupVolume(nodeDepth);
        node->variable(variables::NodeDepth) = nodeDepth;
    }

    if (isZero(V_Pond) || V_Pond <= 0.0)
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

    std::shared_ptr<geometry::Node> node = m_geometry->getNode(m_sinkNodeIdx);

    // Set the downstream boundary condition to be the level pool depth
    // given the current system volume.
    if (!m_realTimeDsHead)
    {
        double depth = getSystemHead(V_I - V_P) - node->getInvert();
        if (depth < 0.0)
            depth = 0;
        node->variable(variables::NodeDepth) = depth;
        node->variable(variables::NodeVolume) = node->lookupVolume(depth);
    }

#if USE_EIGEN
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
#endif
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


#if USE_EIGEN

void ICAP::setInitialMatrixDepthGuess()
{
    // Loop over every node and set an initial guess.
    geometry::NodeList* list = m_geometry->getNodeList();
    for (int i = 0; i < list->count(); i++)
    {
        if (i == m_sinkNodeIdx)
            continue;

        std::shared_ptr<geometry::Node> node = list->get(i);
        m_geometry->setNodeVariable(list->id(i), variables::NodeDepth, 100000);

        geometry::Node::LinkIter iter = node->beginUpstreamLink();
        while (iter != node->endUpstreamLink())
        {
            double diam2 = (*iter)->getMaxDepth() / 2;
            if (diam2 < m_geometry->getNodeVariable(i, variables::NodeDepth))
            {
                m_geometry->setNodeVariable(list->id(i), variables::NodeDepth, diam2);
            }
        }
    }
}


bool ICAP::iterateMatrix()
{
    bool result = true;

    float maxError = 1;

    geometry::LinkList* links = m_geometry->getLinkList();
    geometry::NodeList* nodes = m_geometry->getNodeList();

    Eigen::VectorXf curQ(links->count());
    Eigen::VectorXf curH(nodes->count());

    int linkCount = links->count();
    int nodeCount = nodes->count();

    // Copy values from previous time step into temp vars.
    for (int i = 0; i < linkCount; i++)
        curQ(i) = m_geometry->getLinkVariable(links->id(i), variables::LinkFlow);
    for (int i = 0; i < nodeCount; i++)
        curH(i) = m_geometry->getNodeVariable(nodes->id(i), variables::NodeDepth);

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

        for (int i = 0; i < linkCount; i++)
            curQ(i) += delta(i);;
        for (int i = 0; i < nodeCount; i++)
            curH(i) += delta(i + linkCount);
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
        for (int i = 0; i < linkCount; i++)
            m_geometry->setLinkVariable(links->id(i), variables::LinkFlow, curQ(i));
        for (int i = 0; i < nodeCount; i++)
            m_geometry->setNodeVariable(nodes->id(i), variables::NodeDepth, curH(i));
    }

    return result;
}


bool ICAP::setHfAndDE(Eigen::VectorXf& curQ, Eigen::VectorXf& curH)
{
    bool result = true;
    
    geometry::LinkList* links = m_geometry->getLinkList();
    geometry::NodeList* nodes = m_geometry->getNodeList();

    int linkCount = links->count();
    int nodeCount = nodes->count();

    m_matrixRhs.setZero();

    // Loop over every node and set an initial guess.
    for (int i = 0; i < linkCount; i++)
    {
        const id_type& dsId = links->get(i)->getDownstreamNode()->getId();
        const id_type& usId = links->get(i)->getUpstreamNode()->getId();

        double dsDepth =  m_geometry->getNodeVariable(dsId, variables::NodeDepth);
        double usDepth =  m_geometry->getNodeVariable(usId, variables::NodeDepth);

        double hf = 0;
        if (!m_hpgList.getHf(
            i,
           dsDepth,
            m_geometry->getLinkVariable(links->id(i), variables::LinkFlow), hf))
        {
            return false;
        }

        m_matrixLhs(i, i) = hf;

        m_matrixRhs(i) = hf + dsDepth - usDepth;

        double flow = m_geometry->getLinkVariable(links->id(i), variables::LinkFlow);
        m_matrixRhs(linkCount + dsId) += flow;
        m_matrixRhs(linkCount + usId) -= flow;
    }

    double totalLatFlow = 0;
    for (int i = 0; i < nodeCount; i++)
    {
        double latFlow = m_geometry->getNodeVariable(nodes->id(i), variables::NodeLateralInflow);
        m_matrixRhs(linkCount + i) += latFlow;
        totalLatFlow += latFlow;
    }

    m_matrixRhs(linkCount + m_sinkNodeIdx) -= totalLatFlow;

    for (int i = 0; i < m_matrixRhs.size(); i++)
    {
        m_matrixRhs(i) = - m_matrixRhs(i);
    }

    return result;
}


#endif

bool ICAP::updateOverflows(double routeStep)
{
    bool result = false;

    geometry::NodeList* nodes = m_geometry->getNodeList();
    int nodeCount = nodes->count();

    for (int i = 0; i < nodeCount; i++)
    {
        std::shared_ptr<geometry::Node> node = m_geometry->getNode(nodes->id(i));
        var_type depth = node->variable(variables::NodeDepth);
        var_type maxDepth = node->getMaxDepth();
        if (node->getCanFlood() && depth > maxDepth)
        {
            m_geometry->setNodeVariable(nodes->id(i), variables::NodeOverflow, depth - maxDepth);
            m_geometry->updateNodeStatistic(nodes->id(i), statvariables::FloodedNodes, routeStep);

            if (! m_overflow.IsInEvent(i))
                m_geometry->updateSystemStatistic(statvariables::FloodedNodes, 1.0);

            result = true;
        }
        else
        {
            node->variable(variables::NodeOverflow) = 0;
            m_overflow.Reset(i);
        }
    }

    if (result)
        m_geometry->updateSystemStatistic(statvariables::FloodedNodes, 1.0);

    return result;
}

//
//bool ICAP::RunSimulation(int sinkNodeIdx)
//{
//    using namespace std;
//    
//    double curStep = 0.0;
//    double initDepth = m_geometry->getNode(sinkNodeIdx)->getInitialDepth();
//
//    double deltaT = 0;
//    if (!tryParse(m_geometry->getOption("routing_step"), deltaT))
//    {
//        return false;
//    }
//
//    // Compute the initial volume and water depth of the sink node.
//    std::shared_ptr<geometry::Node> node = m_geometry->getNode(sinkNodeIdx);
//    double volume = node->lookupVolume(initDepth);
//    node->variable(variables::NodeDepth) = initDepth;
//
//    // Now iterate over each timestep and route the system for that step.
//    bool toContinue = true;
//    int iterCount = 0;
//    while (curStep <= m_totalDuration)
//    {
//        if (! Step(&curStep, deltaT, false) )
//        {
//            toContinue = false;
//            break;
//        }
//        iterCount++;
//    }
//
//    return toContinue;
//}
