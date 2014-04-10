#define _CRT_SECURE_NO_DEPRECATE
#define _USE_MATH_DEFINES

#include "icap.h"
#include "util.h"
#include "exception.h"
#include "icap_error.h"
#include "debug.h"
#include <cmath>
#include "routing.h"
#include "Eigen/Dense"

extern "C" TNodeStats*     NodeStats; // Defined in STATS.C


ICAP::ICAP()
{
#ifdef ICAP_DEBUGGING
    m_debugFile = fopen("c:\\temp\\debug_icap.txt", "w");
    if (m_debugFile == NULL)
        m_debugFile = fopen("debug.txt", "w");
#endif

    m_closed = m_ended = true;
    m_hasJunctionCoords = false;

    V_Pond = 0.0;
    V_PondMax = 0.0;
    V_I = 0.0;
    V_P = 0.0;
    V_Ov = 0.0;
    V_SysMax = 0.0;
    m_secondsSinceLastInflow = 0.0;

    m_pumpTSIdx = -1;

	m_counter = 0;

    SetNormCritParamDefaults(m_ncParams);

    // For the purposes of calculating critical depth, the unsteady depth
    // should be slighly less than the crown of the pipe?
    m_ncParams.UnsteadyDepth = 0.95;

    m_regime = 0;

    m_rtMode = false;

    table_init(&m_totalVolumeCurve);
}

ICAP::~ICAP()
{
#ifdef ICAP_DEBUGGING
    if (m_debugFile != NULL)
        fclose(m_debugFile);
#endif
    cleanup();
}


bool ICAP::Open(char *inputFile, char *outputFile, char *reportFile, bool loadhpgs)
{
    bool result = true;

    try
    {

	printf("Loading input file...\n");
    // Load SWMM file.
    if (! loadInputFile(inputFile, reportFile, outputFile))
    {
		report_writeErrorMsg(ERR_INP_FILE, (char*)m_errorStr.c_str());
        ErrorCode = ERR_INPUT;
        cleanup();
        return false;
    }

	printf("Populating network...\n");
    // Populate the ICAP Network object.
    if (! populateNetwork(inputFile))
    {
		report_writeErrorMsg(ERR_ICAP_NETWORK, (char*)m_errorStr.c_str());
        ErrorCode = ERR_INPUT;
        cleanup();
        return false;
    }

    m_overflow.Init();

	printf("Loading HPG's...\n");
    // Load the HPGs.
    if (loadhpgs && !loadHPGs(HPGPath))
    {
		report_writeErrorMsg(ERR_HPG_LOADING, (char*)m_errorStr.c_str());
        ErrorCode = ERR_INPUT;
        cleanup();
        return false;
    }

    }
    catch(...)
    {
        ErrorCode = ERR_SYSTEM;
		report_writeErrorMsg(ERR_SYSTEM, "unable to open ICAP engine");
        result = false;
    }

    return result;
}


bool ICAP::Start(bool buildConnMatrix)
{
    bool result = true;

    try
    {

    // Find all of the inputs to the system.
    if (!m_rtMode)
    {
        int sourceCount = findSources(m_sources);
        if (sourceCount == 0)
        {
		    report_writeErrorMsg(ERR_ICAP_NETWORK, "unable to find any source nodes");
            ErrorCode = ERR_ICAP_NETWORK;
            return false;
        }
    }

    if (IS_ZERO(Node[m_sinkNodeIdx].fullDepth))
    {
        report_writeErrorMsg(ERR_ICAP_NETWORK, "The downstream reservoir max depth must be non-zero");
        ErrorCode = ERR_ICAP_NETWORK;
        return false;
    }

    StepCount = 0;

    int swmmStatus = swmm_start(TRUE); // returns 1 in case of error, 0 if success
    if (swmmStatus)
    {
		report_writeErrorMsg(ERR_SYSTEM, "unable to start SWMM engine");
        ErrorCode = ERR_SYSTEM;
        return false;
    }

    // Start out the event-to-pumping interval counter with the
    // correct duration to allow for pumping from the start of a
    // simulation.
    m_secondsSinceLastInflow = SECS_PER_DAY * DaysBeforePumping;

	// Compute the ponded pipe volume
	V_PondMax = computePondedPipeStorage();

    // Also computes the maximum volume that the system can store.
    V_SysMax = computeTotalVolumeCurve(m_totalVolumeCurve);

#ifdef ICAP_DEBUGGING
	dprintf("V_SysMax=%f V_PondMax=%f\n", V_SysMax, V_PondMax);
#endif

    if (V_SysMax < 0.0)
    {
        std::string err = "Unable to compute total volume curve.";
        error_setInpError(ERR_ICAP_NETWORK, (char*)err.c_str());
        result = false;
    }

    // Set all of the depths to be zero.
    InitializeZeroDepths();

    // Compute the initial volume and water depth of the sink node.
    Node[m_sinkNodeIdx].newDepth = Node[m_sinkNodeIdx].initDepth;
	Node[m_sinkNodeIdx].newVolume = node_getVolume(m_sinkNodeIdx, Node[m_sinkNodeIdx].initDepth);

    if (Node[m_sinkNodeIdx].newDepth > 0.0)
    {
        V_I = computePondedPipeStorage(Node[m_sinkNodeIdx].invertElev + Node[m_sinkNodeIdx].newDepth) +
            Node[m_sinkNodeIdx].newVolume;
    }

    // Reopen the mass balance engine because we've just updated the pip
    // volumes and we need to take those into account.
    massbal_open();

    // Find pumping timeseries curve
    for (int i = 0; i < Nobjects[TSERIES]; i++)
    {
        if (! strcmp(Tseries[i].ID, "PUMPING"))
        {
            m_pumpTSIdx = i;
            break;
        }
    }

	std::cout << buildConnMatrix << std::endl;

	// Build the connectivity matrix for the gradient method
	if (buildConnMatrix)
	{
		using namespace Eigen;

		int numNodes = Nobjects[NODE];
		int numLinks = Nobjects[LINK];

		MatrixXf matrixLhs(numLinks + numNodes, numLinks + numNodes);
		matrixLhs.setZero();

		for (int nl = 0; nl < numLinks; nl++)
		{
			// Set A21 sub-matrix (connectivity)
			matrixLhs(Link[nl].node1 + numLinks, nl) = -1;
			matrixLhs(Link[nl].node2 + numLinks, nl) = 1; 

			// Set A11 sub-matrix (hf diagonal)
			matrixLhs(nl, nl) = 1;

			// Set A12 sub-matrix (transpose of A21)
			matrixLhs(nl, Link[nl].node1 + numLinks) = -1;
			matrixLhs(nl, Link[nl].node2 + numLinks) = 1; 
		}

		this->m_matrixLhs = matrixLhs;

		//std::cout.precision(1);
		//std::cout << matrixLhs.inverse() <<std::endl;
		//getchar();
	}

    }
    catch(...)
    {
        ErrorCode = ERR_SYSTEM;
        result = false;
    }

    return result;
}


void ICAP::SetReservoirDepth(float resDepth)
{
    Node[m_sinkNodeIdx].initDepth = resDepth;
}


int ICAP::GetReservoirNodeIndex()
{
    return this->m_sinkNodeIdx;
}


int ICAP::FindNodeIndex(char* nodeId)
{
    return project_findObject(NODE, nodeId);
}


float ICAP::GetNodeInvert(int nodeIdx)
{
    if (nodeIdx >= 0 && nodeIdx < Nobjects[NODE])
    {
        return Node[nodeIdx].invertElev;
    }
    else
    {
        return -99999;
    }
}


float ICAP::GetNodeWaterElev(int nodeIdx)
{
    if (nodeIdx >= 0 && nodeIdx < Nobjects[NODE])
    {
        return Node[nodeIdx].newDepth + Node[nodeIdx].invertElev;
    }
    else
    {
        return -99999;
    }
}


float ICAP::GetNodeFlow(int nodeIdx)
{
    if (nodeIdx >= 0 && nodeIdx < Nobjects[NODE])
    {
        return getFlowAtNode(nodeIdx);
    }
    else
    {
        return -99999;
    }
}


bool ICAP::loadInputFile(char* inputFile, char* reportFile, char* outputFile)
{
    // Start SWMM and read input file.
    int result = swmm_open(inputFile, outputFile, reportFile);
    if (result)
    {
        std::string err = "loading SWMM file: ";
        err += error_getMsg(result);
        setError(1, err);
        return false;
    }

    // Validate the SWMM network.
    if (! validateNetwork())
    {
        return false;
    }

    return true;
}


// Populate the internal network object.
bool ICAP::populateNetwork(char* inputFile)
{
    int sinkType = 0;
    return populateNetwork(inputFile, m_network, sinkType, m_sinkNodeIdx, m_sinkLinkIdx);
}


// This function is here to make things a little more generic so that it
// doesn't necessarily populate the internal network object.
bool ICAP::populateNetwork(char* inputFile, ICAPNetwork& network, int& sinkType, int& sinkNodeIdx, int& sinkLinkIdx)
{
    // Now populate our network from the SWMM network and read in the coordinates
    // of each node and link.
    populateNetworkFromSWMM(network);

    int coordResult = GetSWMMCoordsAndVerts(inputFile, network);
    if (coordResult <= 0)
        m_hasJunctionCoords = false;
    else
        m_hasJunctionCoords = true;
    
    
    // Now find the sink node.
    sinkType = STORAGE;
    sinkNodeIdx = INVALID_IDX;
    sinkLinkIdx = findSinkLink(sinkType, sinkNodeIdx);

    if (sinkNodeIdx == INVALID_IDX || sinkLinkIdx == INVALID_IDX)
    {
        setError(1, "failed to find a STORAGE or OUTFALL node");
        return false;
    }

    return true;
}


bool ICAP::loadHPGs(char* hpgPath)
{
    // Load the HPGs.
    bool hpgResult = m_hpgList.LoadHPGs(hpgPath);
    if (! hpgResult)
    {   
        std::string err = "(LoadHPGs) HPG's failed to load: ";
        err += m_hpgList.GetErrorStr();
        setError(1, err);
        return false;
    }

    return true;
}


// return < 0 for done, 0 for ok, and > 0 for error
int ICAP::LoadNextHPG()
{
    // Load the HPGs.
    int result = m_hpgList.LoadNextHPG(HPGPath);
    if (result > 0)
    {   
        std::string err = "(LoadNextHPG) HPG's failed to load: ";
        err += m_hpgList.GetErrorStr();
        setError(1, err);
		report_writeErrorMsg(ERR_HPG_LOADING, (char*)m_errorStr.c_str());
        return 1; // return error
    }
    else if (result < 0)
        return -1;
    else
        return 0;
}


bool ICAP::cleanup()
{
    if (! m_ended)
        End();
    if (! m_closed)
        Close();
    
    m_closed =
        m_ended = true;

    return true;
}

