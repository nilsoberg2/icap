#ifndef __ICAP_H____________________________20080424150000__
#define __ICAP_H____________________________20080424150000__

#include <string>

#include "../Eigen/Dense"
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


// Illinois Conveyance Analysis Program
class ICAP_API ICAP : public Parseable
{
private:
    
    ///////////////////////////////////////////////////////////////////////////
    // OBJECTS
    
    // Object that manages the list of HPG's.
    IcapHpg m_hpgList;

    // Object that manages the network topology.
    //std::unique_ptr<IcapGeometry> m_geometry;
    std::shared_ptr<IcapGeometry> m_geometry;
    //std::unique_ptr<Model> m_geometry;

    // This curve stores a pre-computed total system volume curve.
    geometry::Curve m_totalVolumeCurve;

    // Keeps track of overflow status for an event.
    IcapOverflow m_overflow;

	Eigen::MatrixXf m_matrixLhs;

	Eigen::VectorXf m_matrixRhs;

    Pumping m_pumping;

    Results m_results;

    Report m_report;
    
    ///////////////////////////////////////////////////////////////////////////
    // FLAGS

    // Have we ended the SWMM engine?
    bool m_ended;

    // Have we closed the SWMM engine?
    bool m_closed;

    // Keep track of the number of time steps.
	int m_counter;

    // Stores the type of regime that the computation is in currently.
    int m_regime;

    // This variable is true if the model is in Real-Time mode which means that flows
    // are provided externally on a time-step basis rather than reading TS from the 
    // input files.
    bool m_realTimeFlows;

    // This variable is true if the model is in Real-Time mode and a downstream head
    // has been specified.
    bool m_realTimeDsHead;


    ///////////////////////////////////////////////////////////////////////////
    // NODE/LINK ID'S

    // ID of the sink node.
    id_type m_sinkNodeIdx;


    ///////////////////////////////////////////////////////////////////////////
    // MASS-BALANCE VARIABLES

    // Current ponded volume.
    var_type V_Pond;

    // Maximum ponded volume.
    var_type V_PondMax;

    // Total inflow volume, cumulative.
    var_type V_I;

    // Total pumped volume, cumulative.
    var_type V_P;

    // Counter for keeping track of overflowed volume, in a strictly
    // mass-balanced system.
    var_type V_Ov;

    // The maximum volume that the system as a whole can contain.
    var_type V_SysMax;

    // Flag to indicate if this is the first iteration of the matrix.
    bool m_isFirstMatrixIteration;



    ///////////////////////////////////////////////////////////////////////////
    // TIME METHODS

    // Current routing time (seconds)
    double m_newRoutingTime;
    double m_totalDuration;

    // Routing timestep (seconds)
    double m_routeStep;

    // Current reporting time (seconds)
    double m_reportTime;

    // Reporting timestep (seconds)
    int m_reportStep;

    int m_stepCount;

    ///////////////////////////////////////////////////////////////////////////
    // PROPERTIES
    std::string m_hpgPath;

    ///////////////////////////////////////////////////////////////////////////
    // ERROR AND DEBUGGING VARIABLES

    // Internal error code.
    int m_errorCode;

    // Current error in text.
    std::string m_errorStr;

    boost::log::sources::severity_logger<loglevel::SeverityLevel> m_log;

private:

    ///////////////////////////////////////////////////////////////////////////
    // NETWORK FUNCTIONS

    // Loads the input file.
    bool loadInputFile(const std::string&  inputFile);

    // Load all of the HPG's in the HPG list.
    bool loadHpgs(const std::string& hpgPath);

    id_type findFirstNode(geometry::NodeType sinkNodeType);

    // This validates the network.
    bool validateGeometry();


    ///////////////////////////////////////////////////////////////////////////
    // REPORTING AND OUTPUT FUNCTIONS
    
    // Open the report file.
    //TODO
    bool openReport() { return true; }

    //TODO
    void writeReportMessage(const std::string& message) {}

    // Close the report file.
    //TODO
    void closeReport() {}

    void info(const std::string& message) {}
    
    // Open the output file.
    //TODO
    bool openOutput() { return true; }

    // Close the output file.
    //TODO
    void closeOutput() {}


    ///////////////////////////////////////////////////////////////////////////
    // STORAGE AND PUMPING FUNCTIONS

    // Compute the volume stored in the pipes.
	double computePipeStorage();

    // Compute the water volume that can be stored ponded in an adverse-slope
    // pipe network.
	double computePondedPipeStorage();
	double computePondedPipeStorage(var_type h); // optional starting elevation
    
    // Compute a curve of total volume vs. elevation.  This is used for
    // determining the water depth in the pipes for a given storage in the
    // system.
    var_type computeTotalVolumeCurve(geometry::Curve& curve);

    // Determine the head in the system for a given volume.
    var_type getSystemHead(var_type vol);

    
    ///////////////////////////////////////////////////////////////////////////
    // ROUTING FUNCTIONS

    // Do a steady-state routing (calls steadyRouteNode, pondedRouteNode,
    // steadyRouteLink, or pondedRouteLink accordingly).
    bool steadyRoute(const id_type& sinkNodeIdx, bool ponded = false);

    /// <summary>
    /// The goal of this function is to pass a node depth to the downstream end
    /// of upstream conduits.  Node depths can be different than conduit depths
    /// because of transition losses in junctions or geometry changes.
    /// </summary>
    bool steadyRouteNode(const id_type& nodeIdx, bool isPonded);

    // Do a steady-state routing for a link.
    bool steadyRouteLink(const id_type& linkIdx);

    // Do a ponded routing for a node (ponded = no flow).
    //bool pondedRouteNode(const id_type& nodeIdx);

    // Do a ponded routing for a link (ponded = no flow).
    bool pondedRouteLink(const id_type& linkIdx);


    ///////////////////////////////////////////////////////////////////////////
    // NODE/LINK ATTRIBUTE ACCESS FUNCTIONS

    // Update the overflow value for each node.
    bool updateOverflows(double routeStep);

    // Get the inflow at the given node.
    var_type getFlowAtNode(const id_type& idx);


    ///////////////////////////////////////////////////////////////////////////
    // JUNCTION LOSS FUNCTIONS

    // Compute the loss at the given node.  Updates the water depth at that node.
    bool computeNodeLosses(const id_type& nodeId);


    ///////////////////////////////////////////////////////////////////////////
    // INIT, DE-INIT, ERROR, AND DEBUGGING FUNCTIONS

    // Cleanup the this object and the SWMM engine in case of abnormal termination.
    bool cleanup();


    ///////////////////////////////////////////////////////////////////////////
    // REPORTING/RESULTS

    void updateTimestepStatistics() {};

    void saveTimestepResults() {};

    ///////////////////////////////////////////////////////////////////////////
    // ITERATION FUNCTIONS

    // This is called when pumping from the system.
    bool Step_Draining();

    // This is called when the ponded area of the tunnels is filling.
    bool Step_PondedFilling();

    bool Step_SteadyState(bool useMatrix);

    bool iterateMatrix();

    void setInitialMatrixDepthGuess();

    bool setHfAndDE(Eigen::VectorXf& curQ, Eigen::VectorXf& curH);

public:
    ICAP();
    ~ICAP();

    void InitializeLog(loglevel::SeverityLevel level, std::string logFilePath);

    // This opens the input file, loads everything, and opens the SWMM engine.
    bool Open(const std::string& inputFile, const std::string& outputFile, const std::string& reportFile, bool loadhpgs);

    // This performs some initialization (finding of sources, computation of
    // total volume curve, etc.).
    bool Start(bool buildConnMatrix = false);
    
    // This performs the computations for the next timestep.
    bool Step(double* elapsedTime, bool useMatrix);

    // This performs the computations for the next timestep.
    bool Step(double* elapsedTime, double routeStep, bool useMatrix);

    // This is the counterpart of Start() and closes up some loose ends.
    bool End();

    // This is the counterpart of Open() and closes up more loose ends.
    bool Close();

    // This loads the next HPG in the list.  The list comes from the conduit
    // list.  Returns < 0 for done, 0 for ok, and > 0 for error
    int loadNextHpg();

    int GetLinkCount();

    // Run an entire simulation.
    //bool RunSimulation(int sinkNodeIdx);

	// Save total volume curve
	void SaveTotalVolumeCurve(const std::string& file);

    const id_type& GetReservoirNodeIndex();

    const id_type& FindNodeIndex(const std::string& nodeId);

    var_type GetNodeInvert(const id_type& nodeIdx);

    var_type GetNodeWaterElev(const id_type& nodeIdx);

    var_type GetNodeFlow(const id_type& nodeIdx);

    // Initialize the depths to be zero in all of the nodes and links.
	void InitializeZeroDepths();

    void InitializeZeroFlows();

    ///////////////////////////////////////////////////////////////////////////
    // Real-time control functions

    /// <summary>
    /// [FEEDBACK METHOD] Enable real-time control (e.g. feedback).  Once enabled there
    /// is no going back.
    /// </summary>
    void EnableRealTimeStatus();

    /// <summary>
    /// [FEEDBACK METHOD] Set the current node inflow (override any inflow objects).
    /// If the node is invalid then this method does nothing.
    /// </summary>
    void SetCurrentNodeInflow(const std::string& nodeId, var_type flow);

    /// <summary>
    /// [FEEDBACK METHOD] Set the current node inflow (override any inflow objects).
    /// If the node is invalid then this method does nothing.
    /// </summary>
    void SetCurrentNodeHead(const std::string& nodeId, var_type head);

    /// <summary>
    /// [FEEDBACK METHOD] Return the head at the given node.  Returns a value less
    /// than or equal to -99999 if the node is invalid.
    /// </summary>
    var_type GetCurrentNodeHead(const std::string& nodeId);

    /// <summary>
    /// [FEEDBACK METHOD] Return the total inflow at the given node.  Returns a value less
    /// than or equal to -99999 if the node is invalid.
    /// </summary>
    var_type GetCurrentNodeInflow(const std::string& nodeId);

    /// <summary>
    /// [FEEDBACK METHOD] Set a specific node as an input node.
    /// </summary>
    void AddSource(const std::string& nodeId);

    // Set the reservoir depth programmatically
    void SetReservoirDepth(var_type resDepth);

    // Set the flow factor (scale factor on all of the constant flows)
    void SetFlowFactor(var_type flowFactor);

    double GetTotalDuration();
};


#endif//__ICAP_H____________________________20080424150000__
