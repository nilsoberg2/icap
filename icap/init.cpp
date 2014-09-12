#define _CRT_SECURE_NO_DEPRECATE

#define _USE_MATH_DEFINES
#include <cmath>

#include "../Eigen/Dense"
#include "../util/math.h"

#include "icap.h"
#include "routing.h"


ICAP::ICAP()
    : m_totalVolumeCurve("")
{
    m_closed = m_ended = true;

    V_Pond = 0.0;
    V_PondMax = 0.0;
    V_I = 0.0;
    V_P = 0.0;
    V_Ov = 0.0;
    V_SysMax = 0.0;
	m_counter = 0;
    m_regime = 0;
    m_realTimeFlows = false;
    m_realTimeDsHead = false;
    m_isFirstMatrixIteration = false;
    m_geometry = NULL;
}

ICAP::~ICAP()
{
    cleanup();
    delete m_geometry;
}


int ICAP::GetLinkCount()
{
    if (m_geometry != NULL && m_geometry->getLinkList() != NULL)
    {
        return m_geometry->getLinkList()->count();
    }
    else
    {
        return 0;
    }
}


void ICAP::InitializeLog(loglevel::SeverityLevel level, std::string logFilePath)
{
    boost::log::add_file_log(logFilePath);
    boost::log::add_common_attributes();
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= level);
}


bool ICAP::Open(const std::string& inputFile, const std::string& outputFile, const std::string& reportFile, bool loadhpgs)
{
    bool result = true;

    //try
    {
	    info("Loading input file...");
        // Load SWMM file.
        //m_geometry = new IcapGeometry();
        if (! loadInputFile(inputFile))
        {
            return false;
        }
    }
    //catch (...)
    //{
    //    setErrorMessage("Failed to load input file due to a system exception.");
    //    return false;
    //}

    m_overflow.Init(m_geometry->getNodeList());

    //try
    {
	    info("Loading HPG's...");
        // Load the HPGs.
        if (loadhpgs)
        {
            m_hpgPath = m_geometry->getHpgPath();
            if (!loadHpgs(m_hpgPath))
            {
                return false;
            }
        }
    }
    //catch(...)
    //{
    //    setErrorMessage("Exception occured when loading HPGs.");
    //    result = false;
    //}

    if (result && m_realTimeFlows)
    {
        m_geometry->enableRealTimeStatus();
    }

    return result;
}


bool ICAP::Start(bool buildConnMatrix)
{
    bool result = true;

    try
    {
        if (isZero(m_geometry->getNode(m_sinkNodeIdx)->getMaxDepth()))
        {
		    BOOST_LOG_SEV(m_log, loglevel::error) << "The downstream reservoir max depth must be non-zero";
            return false;
        }
    }
    catch(...)
    {
    }

    m_stepCount = 0;

	// Compute the ponded pipe volume
	V_PondMax = computePondedPipeStorage();

    // Also computes the maximum volume that the system can store.
    V_SysMax = computeTotalVolumeCurve(m_totalVolumeCurve);

    BOOST_LOG_SEV(m_log, loglevel::debug) << "V_SysMax=" << V_SysMax << " V_PondMax=" << V_PondMax;

    if (V_SysMax < 0.0)
    {
        BOOST_LOG_SEV(m_log, loglevel::error) << "Unable to compute total volume curve.";
        result = false;
    }

    // Set all of the depths to be zero.
    InitializeZeroDepths();

    // Compute the initial volume and water depth of the sink node.
    std::shared_ptr<geometry::Node> node = m_geometry->getNode(m_sinkNodeIdx);
    var_type initDepth = node->getInitialDepth();
    m_geometry->setNodeVariable(m_sinkNodeIdx, variables::NodeDepth, initDepth);
    m_geometry->setNodeVariable(m_sinkNodeIdx, variables::NodeVolume, node->lookupVolume(initDepth));

    if (initDepth > 0.0)
    {
        V_I = computePondedPipeStorage(node->getInvert() + initDepth) + node->lookupVolume(initDepth);
    }

    if (!m_pumping.initializeSettings(m_geometry))
    {
        appendErrorMessage(m_pumping.getErrorMessage());
        return false;
    }

    //try
    //{
	    // Build the connectivity matrix for the gradient method
	    if (buildConnMatrix)
	    {
		    using namespace Eigen;

            geometry::NodeList* nodes = m_geometry->getNodeList();
            geometry::LinkList* links = m_geometry->getLinkList();
		    int numNodes = nodes->count();
		    int numLinks = links->count();

		    MatrixXf matrixLhs(numLinks + numNodes, numLinks + numNodes);
		    matrixLhs.setZero();

		    for (int nl = 0; nl < numLinks; nl++)
		    {
			    // Set A21 sub-matrix (connectivity)
                std::shared_ptr<geometry::Link> link = links->get(nl);
			    matrixLhs(link->getUpstreamNode()->getId() + numLinks, nl) = -1;
			    matrixLhs(link->getDownstreamNode()->getId() + numLinks, nl) = 1; 

			    // Set A11 sub-matrix (hf diagonal)
			    matrixLhs(nl, nl) = 1;

			    // Set A12 sub-matrix (transpose of A21)
			    matrixLhs(nl, link->getUpstreamNode()->getId() + numLinks) = -1;
			    matrixLhs(nl, link->getDownstreamNode()->getId() + numLinks) = 1; 
		    }

		    this->m_matrixLhs = matrixLhs;

            this->m_matrixRhs = VectorXf(numLinks + numNodes);

		    //std::cout.precision(1);
		    //std::cout << matrixLhs.inverse() <<std::endl;
		    //getchar();
	    }
    //}
    //catch(...)
    //{
    //    BOOST_LOG_SEV(m_log, loglevel::error) << "Unable to generate connectivity matrix.";
    //    result = false;
    //}

    return result;
}


void ICAP::SetReservoirDepth(var_type resDepth)
{
    m_geometry->getNode(m_sinkNodeIdx)->setInitialDepth(resDepth);
}


const id_type& ICAP::GetReservoirNodeIndex()
{
    return this->m_sinkNodeIdx;
}


const id_type& ICAP::FindNodeIndex(const std::string& nodeId)
{
    return m_geometry->getNode(nodeId)->getId();
}


var_type ICAP::GetNodeInvert(const id_type& nodeIdx)
{
    if (nodeIdx >= 0 && nodeIdx < m_geometry->getNodeList()->count())
    {
        return m_geometry->getNode(nodeIdx)->getInvert();
    }
    else
    {
        return -99999;
    }
}


var_type ICAP::GetNodeWaterElev(const id_type& nodeIdx)
{
    if (nodeIdx >= 0 && nodeIdx < m_geometry->getNodeList()->count())
    {
        return m_geometry->getNodeVariable(nodeIdx, variables::NodeDepth) + m_geometry->getNode(nodeIdx)->getInvert();
    }
    else
    {
        return -99999;
    }
}


var_type ICAP::GetNodeFlow(const id_type& nodeIdx)
{
    if (nodeIdx >= 0 && nodeIdx < m_geometry->getNodeList()->count())
    {
        return getFlowAtNode(nodeIdx);
    }
    else
    {
        return -99999;
    }
}


// Load the input file and 
bool ICAP::loadInputFile(const std::string& inputFile)
{
    m_geometry = new IcapGeometry();
    if (!m_geometry->loadFromFile(inputFile, geometry::FileFormatSwmm5))
    {
        BOOST_LOG_SEV(m_log, loglevel::error) << m_geometry->getErrorMessage();
        return false;
    }

    // Validate the network.
    if (! validateGeometry())
    {
        return false;
    }

    return true;
}


bool ICAP::loadHpgs(const std::string& hpgPath)
{
    // Load the HPGs.
    if (! m_hpgList.loadHpgs(hpgPath, m_geometry->getLinkList()))
    {   
        BOOST_LOG_SEV(m_log, loglevel::error) << "HPG's failed to load: " << m_hpgList.getErrorMessage();
        return false;
    }

    return true;
}


int ICAP::loadNextHpg()
{
    // Load the HPGs.
    int result = m_hpgList.loadNextHpg(m_hpgPath, m_geometry->getLinkList());
    if (result > 0)
    {   
        BOOST_LOG_SEV(m_log, loglevel::error) << "(loadNextHpg) HPG's failed to load: " << m_hpgList.getErrorMessage();
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

