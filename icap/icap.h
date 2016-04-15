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

#ifndef __ICAP_H____________________________20080424150000__
#define __ICAP_H____________________________20080424150000__

#include <string>

#ifdef USE_EIGEN
#include "../deps/Eigen/Dense"
#endif

#include "../time/datetime.h"
#include "../util/parseable.h"
#include "../api.h"

#include "hpg.h"
#include "routing.h"
#include "benchmark.h"
#include "overflow.h"
#include "pumping.h"
#include "logging.h"
#include "output.h"
#include "icap_geometry.h"


/// The ICAP class encapsulates the computational model that determines the conveyance of a system.
/// It relies on the geometry, hpg_create, hpg_interp, model, time, util, and xslib libraries.
class ICAP_API ICAP : public Parseable
{

private:
    
    /// Object that manages the list of HPG's.
    IcapHpg m_hpgList;

    /// Object that manages the network topology.
	/// This is a shared_ptr because we need to share it with other objects at various points.
    std::shared_ptr<IcapGeometry> m_geometry;

    /// This curve stores a pre-computed total system volume curve (F_vt).
    geometry::Curve m_totalVolumeCurve;

    /// Keeps track of overflow status for an event.
    IcapOverflow m_overflow;

#ifdef USE_EIGEN
	Eigen::MatrixXf m_matrixLhs;
	Eigen::VectorXf m_matrixRhs;
#endif

	/// This object encapsulates pumping functionality.
    Pumping m_pumping;

	/// This object encapsulates results storage functionality.
	Results m_results;

	/// This object encapsulates report generation functionality.
	Report m_report;
    
    ///////////////////////////////////////////////////////////////////////////
    // FLAGS

    /// Have we ended the SWMM engine?
    bool m_ended;

    /// Have we closed the SWMM engine?
    bool m_closed;

    /// Keep track of the number of time steps.
	int m_counter;

    /// Stores the type of regime that the computation is in currently.
    int m_regime;

    /// This variable is true if the model is in Real-Time mode which means that flows
    /// are provided externally on a time-step basis rather than reading TS from the 
    /// input files.
    bool m_realTimeFlows;

    /// This variable is true if the model is in Real-Time mode and a downstream head
    /// has been specified.
    bool m_realTimeDsHead;


    ///////////////////////////////////////////////////////////////////////////
    // NODE/LINK ID'S

    /// ID of the sink node.
    id_type m_sinkNodeIdx;


    ///////////////////////////////////////////////////////////////////////////
    // MASS-BALANCE VARIABLES

    /// Current ponded volume.
    var_type V_Pond;

    /// Maximum ponded volume.
    var_type V_PondMax;

    /// Total inflow volume, cumulative.
    var_type V_I;

    /// Total pumped volume, cumulative.
    var_type V_P;

    /// Counter for keeping track of overflowed volume, in a strictly mass-balanced system.
    var_type V_Ov;

    /// The maximum volume that the system as a whole can contain.
    var_type V_SysMax;

#ifdef USE_EIGEN
    /// Flag to indicate if this is the first iteration of the matrix.
    bool m_isFirstMatrixIteration;
#endif


    ///////////////////////////////////////////////////////////////////////////
    // TIME-RELATED VARIABLES

    /// Current time step (seconds).
    double m_newRoutingTime;
    double m_totalDuration;

    /// The computation delta-T time step (seconds).
    double m_routeStep;

    /// This is the next time to report results at (seconds).
    double m_reportTime;

    /// The reporting delta-T time step (seconds).
    int m_reportStep;

	/// The number of steps taken already.
    int m_stepCount;

    ///////////////////////////////////////////////////////////////////////////
    // PROPERTIES

	/// The path to the directory containing HPGs.
    std::string m_hpgPath;

    ///////////////////////////////////////////////////////////////////////////
    // ERROR AND DEBUGGING VARIABLES

    /// Internal error code.
    int m_errorCode;

    /// Current error in human-readable format.
    std::string m_errorStr;

	/// Logging object so we can log information about the simulation.
    boost::log::sources::severity_logger<loglevel::SeverityLevel> m_log;

private:

    ///////////////////////////////////////////////////////////////////////////
    // NETWORK FUNCTIONS

    /// Loads the input file.
    bool loadInputFile(const std::string&  inputFile);

    /// Load all of the HPG's in the HPG list.
    bool loadHpgs(const std::string& hpgPath);

	/// Finds the first node (e.g. the downstream-most node) in the system.
    id_type findFirstNode(geometry::NodeType sinkNodeType);

    /// This validates the network.
    bool validateGeometry();


    ///////////////////////////////////////////////////////////////////////////
    // REPORTING AND OUTPUT FUNCTIONS
    
    /// Open the report file.
    //TODO
    bool openReport() { return true; }

	/// Writes a message to the report file.
    //TODO
    void writeReportMessage(const std::string& message) {}

    /// Close the report file.
    //TODO
    void closeReport() {}
    
    /// Open the output file.
    //TODO
    bool openOutput() { return true; }

    /// Close the output file.
    //TODO
    void closeOutput() {}

	void info(const std::string& message) {}


    ///////////////////////////////////////////////////////////////////////////
    // STORAGE AND PUMPING FUNCTIONS

    /// Computes the volume stored in the pipes.
	double computePipeStorage();

    /// Compute the water volume that can be stored ponded in an adverse-slope pipe network.
	double computePondedPipeStorage();
	/// Compute the water volume that can be stored ponded in an adverse-slope pipe network,
	/// with an optional starting elevation.
	double computePondedPipeStorage(var_type h); 
    
    /// Compute a curve of total volume vs. elevation.  This is used for determining the water
	/// depth in the pipes for a given storage in the system.
    var_type computeTotalVolumeCurve(geometry::Curve& curve);

    /// Determine the head in the system for a given volume, using F_vt
    var_type getSystemHead(var_type vol);

    
    ///////////////////////////////////////////////////////////////////////////
    // ROUTING FUNCTIONS

    /// Do a steady-state routing (calls steadyRouteNode, pondedRouteNode, steadyRouteLink,
	/// or pondedRouteLink accordingly).
    bool steadyRoute(const id_type& sinkNodeIdx, bool ponded = false);

    /// The goal of this function is to pass a node depth to the downstream end
    /// of upstream conduits.  Node depths can be different than conduit depths
    /// because of transition losses in junctions or geometry changes.
    bool steadyRouteNode(const id_type& nodeIdx, bool isPonded);

    /// Do a steady-state routing for a link.
    bool steadyRouteLink(const id_type& linkIdx);

    // Do a ponded routing for a node (ponded = no flow).
    //bool pondedRouteNode(const id_type& nodeIdx);

    /// Do a ponded routing for a link (ponded = no flow).
    bool pondedRouteLink(const id_type& linkIdx);


    ///////////////////////////////////////////////////////////////////////////
    // NODE/LINK ATTRIBUTE ACCESS FUNCTIONS

    /// Update the overflow value for each node.
    bool updateOverflows(double routeStep);

    /// Get the inflow at the given node.
    var_type getFlowAtNode(const id_type& idx);


    ///////////////////////////////////////////////////////////////////////////
    // JUNCTION LOSS FUNCTIONS

    /// Compute the loss at the given node.  Updates the water depth at that node.
    bool computeNodeLosses(const id_type& nodeId);


    ///////////////////////////////////////////////////////////////////////////
    // INIT, DE-INIT, ERROR, AND DEBUGGING FUNCTIONS

    /// Cleanup the this object and the SWMM engine in case of abnormal termination.
    bool cleanup();


    ///////////////////////////////////////////////////////////////////////////
    // REPORTING/RESULTS

	//TODO
    void updateTimestepStatistics() {};

	//TODO
    void saveTimestepResults() {};

    ///////////////////////////////////////////////////////////////////////////
    // ITERATION FUNCTIONS

    /// This iteration method is called when pumping from the system.
    bool Step_Draining();

    /// This iteration method is called when the ponded area of the tunnels is filling.
    bool Step_PondedFilling();

	/// This iteration method is called when the system is in a steady-state ready for routing.
    bool Step_SteadyState(bool useMatrix);


#if USE_EIGEN
    bool iterateMatrix();

    void setInitialMatrixDepthGuess();

    bool setHfAndDE(Eigen::VectorXf& curQ, Eigen::VectorXf& curH);
#endif

public:
    ICAP();
    ~ICAP();

	/// Initializes the logging engine with the specified severity level.
    void InitializeLog(loglevel::SeverityLevel level, std::string logFilePath);

    /// This opens the input file, loads everything, and opens the SWMM engine.
    bool Open(const std::string& inputFile, const std::string& outputFile, const std::string& reportFile, bool loadhpgs);

    /// This performs some initialization (finding of sources, computation of total volume curve, etc.).
    bool Start(bool buildConnMatrix = false);
    
    /// This performs the computations for the next timestep.
    bool Step(double* elapsedTime, bool useMatrix);

    /// This performs the computations for the next timestep.
    bool Step(double* elapsedTime, double routeStep, bool useMatrix);

    /// This is the counterpart of Start() and closes up some loose ends.
    bool End();

    /// This is the counterpart of Open() and closes up more loose ends.
    bool Close();

    /// This loads the next HPG in the list.  The list comes from the conduit
    /// list.  Returns < 0 for done, 0 for ok, and > 0 for error
    int loadNextHpg();

	/// Returns the number of pipes (links) in the system.
    int GetLinkCount();

	/// Compute the total volume curve F_vt and save it to a file for use by someone else.
	void SaveTotalVolumeCurve(const std::string& file);

	/// Returns the index/ID of the downstream-most node.
    const id_type& GetReservoirNodeIndex();

	/// Returns the index for the given node name.
    const id_type& FindNodeIndex(const std::string& nodeId);

	/// Returns the invert elevation for the given node index/ID.
    var_type GetNodeInvert(const id_type& nodeIdx);

	/// Returns the current water elevation (absolute) for the given node index/ID.
	var_type GetNodeWaterElev(const id_type& nodeIdx);

	/// Returns the current flow rate into the given node index/ID.
	var_type GetNodeFlow(const id_type& nodeIdx);

    /// Initialize the depths to be zero in all of the nodes and links.
	void InitializeZeroDepths();

	/// Initialize the flows to be zero in all of the nodes and links.
	void InitializeZeroFlows();

    ///////////////////////////////////////////////////////////////////////////
    // REAL-TIME CONTROL FUNCTIONS

    /// [FEEDBACK METHOD] Enable real-time control (e.g. feedback).  Once enabled there
    /// is no going back.
    void EnableRealTimeStatus();

    /// [FEEDBACK METHOD] Set the current node inflow (override any inflow objects).
    /// If the node is invalid then this method does nothing.
    void SetCurrentNodeInflow(const std::string& nodeId, var_type flow);

    /// [FEEDBACK METHOD] Set the current node inflow (override any inflow objects).
    /// If the node is invalid then this method does nothing.
    void SetCurrentNodeHead(const std::string& nodeId, var_type head);

    /// [FEEDBACK METHOD] Return the head at the given node.  Returns a value less
    /// than or equal to -99999 if the node is invalid.
    var_type GetCurrentNodeHead(const std::string& nodeId);

    /// [FEEDBACK METHOD] Return the total inflow at the given node.  Returns a value less
    /// than or equal to -99999 if the node is invalid.
    var_type GetCurrentNodeInflow(const std::string& nodeId);

    /// [FEEDBACK METHOD] Set a specific node as an input node.
    void AddSource(const std::string& nodeId);

    /// Set the reservoir depth programmatically
    void SetReservoirDepth(var_type resDepth);

    /// Set the flow factor (scale factor on all of the constant flows)
    void SetFlowFactor(var_type flowFactor);

	/// Returns the total duration of the simulation.
    double GetTotalDuration();
};


#endif//__ICAP_H____________________________20080424150000__
