#ifndef __ICAP_H____________________________20080424150000__
#define __ICAP_H____________________________20080424150000__

#include <string>
#include <normcrit.h>

#include "common.h"
#include "source_list.h"
#include "icapnetwork.h"
#include "hpg.h"
#include "routing.h"
#include "debug.h"
#include "icap_math.h"
#include "benchmark.h"
#include "overflow.h"
#include "Eigen/Dense"


// Illinois Conveyance Analysis Program
class ICAP
#ifdef BENCHMARKYES
    : public BenchmarkVariables
#endif
{
protected:
    
    ///////////////////////////////////////////////////////////////////////////
    // OBJECTS

    // Object that manages the list of input sources (e.g. dropshafts).
    SourceList m_sources;
    
    // Object that manages the list of HPG's.
    ICAPHPG m_hpgList;

    // Object that manages the network topology.
    ICAPNetwork m_network;

    // Parameters for the normal/critical depth solver.
    NormCritParams m_ncParams;

    // This curve stores a pre-computed total system volume curve.
    TTable m_totalVolumeCurve;

    // Keeps track of overflow status for an event.
    ICAPOverflow m_overflow;

	Eigen::MatrixXf m_matrixLhs;
    
    ///////////////////////////////////////////////////////////////////////////
    // FLAGS

    // Have we ended the SWMM engine?
    bool m_ended;

    // Have we closed the SWMM engine?
    bool m_closed;

    // Keep track of the number of time steps.
	int m_counter;

    // This variable indicates if the model was able to find junction
    // coordinates.  If not, the model assumes 90 deg lateral angle.
    bool m_hasJunctionCoords;

    // Stores the type of regime that the computation is in currently.
    int m_regime;

    // This variable is true if the model is in Real-Time mode which means that flows
    // are provided externally on a time-step basis rather than reading TS from the 
    // input files.
    bool m_rtMode;


    ///////////////////////////////////////////////////////////////////////////
    // NODE/LINK ID'S

    // Index into SWMM's Node[] array for the reservoir.
    int m_sinkNodeIdx;

    // Index into SWMM's Link[] array for the link just upstream of the reservoir.
    int m_sinkLinkIdx;

    // Index into SWMM's Curve[] array for pumping timeseries record.
    int m_pumpTSIdx;


    ///////////////////////////////////////////////////////////////////////////
    // MASS-BALANCE VARIABLES

    // Current ponded volume.
    double V_Pond;

    // Maximum ponded volume.
    double V_PondMax;

    // Total inflow volume, cumulative.
    double V_I;

    // Total pumped volume, cumulative.
    double V_P;

    // Counter for keeping track of overflowed volume, in a strictly
    // mass-balanced system.
    double V_Ov;

    // The maximum volume that the system as a whole can contain.
    double V_SysMax;

    // Number of seconds since the last inflow to the system occured.
	double m_secondsSinceLastInflow;


    ///////////////////////////////////////////////////////////////////////////
    // ERROR AND DEBUGGING VARIABLES

    // Internal error code.
    int m_errorCode;

    // Current error in text.
    std::string m_errorStr;

#ifdef ICAP_DEBUGGING
    FILE* m_debugFile;
#endif


protected:

    ///////////////////////////////////////////////////////////////////////////
    // NETWORK FUNCTIONS

    // Loads the input file and opens the SWMM engine.
    bool loadInputFile(char* inputFile, char* reportFile, char* outputFile);

    // Load all of the HPG's in the HPG list.
    bool loadHPGs(char* hpgPath);

    // Populate the internal network object.
    bool populateNetwork(char* inputFile);

    // This function is here to make things a little more generic so that it
    // doesn't necessarily populate the internal network object.
    bool populateNetwork(char* inputFile, ICAPNetwork& network, int& sinkType, int& sinkNode, int& sinkLink);

    // This pre-populates the internal network object with the node/link
    // info from SWMM.
    void populateNetworkFromSWMM(ICAPNetwork& network);

    // These two functions find the downstream-most link and node.
    int findSinkLink(int sinkNodeType, int& sinkNodeIdx);
    int findFirstNode(int sinkNodeType, int& subIndex);

    // This validates the network.
    bool validateNetwork();


    ///////////////////////////////////////////////////////////////////////////
    // STORAGE AND PUMPING FUNCTIONS

    // Compute the volume stored in the pipes.
	double computePipeStorage();

    // Compute the water volume that can be stored ponded in an adverse-slope
    // pipe network.
	double computePondedPipeStorage();
	double computePondedPipeStorage(double h); // optional starting elevation

    // Compute the static pumped volume (using the PumpingRate value from the
    // GUI) or get the pumped volume using the timeseries if it is present.
    double computePumpedVolume(double flowAtRes, double currentDate);

    // Get the pumping rate from the pumping time series for the given date.
    double getPumpedRate(double date);

    // Get the pumped volume using the pumping time series for the given date.
    double getPumpedVolume(double date);
    
    // Compute a curve of total volume vs. elevation.  This is used for
    // determining the water depth in the pipes for a given storage in the
    // system.
    double computeTotalVolumeCurve(TTable& curve);

    // Determine the head in the system for a given volume.
    double getSystemHead(double vol);


    ///////////////////////////////////////////////////////////////////////////
    // INFLOW FUNCTIONS

    // Find all of the locations where flow is input to the system and store
    // them in a SourceList object.
    int findSources(SourceList& sourceList);

    void setFlowsFromSource(int node, int link);
    void addExternalInflows(DateTime currentDate);

    // Set inflows for each node for the given date.  Returns the total inflow.
    void initializeFlows(double curDate);

    // Propogate flows downstream from their input node to the reservoir.
    void propogateFlows();


    ///////////////////////////////////////////////////////////////////////////
    // ROUTING FUNCTIONS

    // Do a steady-state routing (calls steadyRouteNode, pondedRouteNode,
    // steadyRouteLink, or pondedRouteLink accordingly).
    bool steadyRoute(int sinkNodeIdx, bool ponded = false);

    // Do a steady-state routing for a node.
    bool steadyRouteNode(int nodeIdx);

    // Do a steady-state routing for a link.
    bool steadyRouteLink(int linkIdx);

    // Do a ponded routing for a node (ponded = no flow).
    bool pondedRouteNode(int nodeIdx);

    // Do a ponded routing for a link (ponded = no flow).
    bool pondedRouteLink(int linkIdx);


    ///////////////////////////////////////////////////////////////////////////
    // NODE/LINK ATTRIBUTE ACCESS FUNCTIONS

    // Updates the water DEPTH of the upstream end of a pipe, and the junction
    // immediately upstream of that pipe. Depth is relative to the pipe/junction
    // invert.
    void updateUpstreamDepthForLink(int linkIdx, double usDepth, double volume);

    // Updates the water ELEVATION of the upstream end of a pipe, and the
    // junction immediately upstream of that pipe. Elevation is absolute, based
    // on a datum.
    void updateUpstreamElevationForLink(int linkIdx, double usElev, double volume);

    // Updates the downstream water DEPTH of a pipe (upstream of a node).
    // Depth is relative to the pipe invert.
    void updateUpstreamDepthForNode(int linkIdx, double dsDepth);

    // Updates the downstream water ELEVATION of a pipe (upstream of a node).
    // Elevation is absolute, based on a datum.
    void updateUpstreamElevationForNode(int linkIdx, double dsElev);

    // Converts an elevation value to a depth (depth = elevation - invert).
    double elevationToDepthForNode(int nodeIdx);
    double elevationToDSDepthForLink(int linkIdx);
    double elevationToUSDepthForLink(int linkIdx);

    // Update the overflow value for each node.
    bool updateOverflows();

    // Get the inflow at the given node.
    double getFlowAtNode(int idx);


    ///////////////////////////////////////////////////////////////////////////
    // JUNCTION LOSS FUNCTIONS

    // Compute the loss at the given node.  Updates the water depth at that node.
    bool computeNodeLosses(int nodeIdx, ICAPNode* node);

    // Compute the angle between two pipes upstream of the junction.
    bool computeAngle(int downIdx, int mainIdx, int latIdx, double* angle);


    ///////////////////////////////////////////////////////////////////////////
    // INIT, DE-INIT, ERROR, AND DEBUGGING FUNCTIONS

    // Cleanup the this object and the SWMM engine in case of abnormal termination.
    bool cleanup();

    // Set the error code and text.
    void setError(int code, std::string errorStr);
    void setError(int code, const char* errorStr);


    ///////////////////////////////////////////////////////////////////////////
    // ITERATION FUNCTIONS

    // This is called when pumping from the system.
    bool Step_Draining();

    // This is called when the ponded area of the tunnels is filling.
    bool Step_PondedFilling();

    bool Step_SteadyState();

public:
    ICAP();
    ~ICAP();

    // This opens the input file, loads everything, and opens the SWMM engine.
    bool Open(char* inputFile, char* outputFile, char* reportFile, bool loadhpgs);

    // This performs some initialization (finding of sources, computation of
    // total volume curve, etc.).
    bool Start(bool buildConnMatrix = false);

    // This performs the computations for the next timestep.
    bool Step(double* curStep_in);

	// This uses the gradient method for computations for the next time step.
	bool StepMatrix(double* curStep_in);

    // This is the counterpart of Start() and closes up some loose ends.
    bool End();

    // This is the counterpart of Open() and closes up more loose ends.
    bool Close();

    // This loads the next HPG in the list.  The list comes from the conduit
    // list.  Returns < 0 for done, 0 for ok, and > 0 for error
    int LoadNextHPG();

    // Run an entire simulation.
    bool RunSimulation(int sinkNodeIdx);

    // Return the error text.
    const char* GetErrorStr();

	// Save total volume curve
	void SaveTotalVolumeCurve(char* file);

    // Set the reservoir depth programmatically
    void SetReservoirDepth(float resDepth);

    // Set the flow factor (scale factor on all of the constant flows)
    void SetFlowFactor(float flowFactor);

    int GetReservoirNodeIndex();

    int FindNodeIndex(char* nodeId);

    float GetNodeInvert(int nodeIdx);

    float GetNodeWaterElev(int nodeIdx);

    float GetNodeFlow(int nodeIdx);

    // Initialize the flow and volumes to be zero in all of the nodes and links.
	void InitializeZeroFlows();

    // Initialize the depths to be zero in all of the nodes and links.
	void InitializeZeroDepths();

    // Set the real-time control status to true or false
    void SetRealTimeStatus(bool enabled);

    void SetCurrentNodeInflow(char* nodeId, double flow);

    double GetCurrentNodeHead(char* nodeId);

    // Specify that the given node acts as a source
    void AddSource(char* nodeId);
};


#endif//__ICAP_H____________________________20080424150000__
