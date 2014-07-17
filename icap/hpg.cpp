#define _CRT_SECURE_NO_DEPRECATE

#include <cmath>
#include <cstring>
#include <sstream>
#include <boost/filesystem.hpp>

#include <normcrit.h>
#include <circhpg.hpp>
#include <hpg/error.hpp>
#include <hpg/math.hpp>

#include "hpg.h"
#include "routing.h"
#include "icap.h"





IcapHpg::~IcapHpg()
{
    //Free();
}

IcapHpg::IcapHpg()
{
    //SetNormCritParamDefaults(m_ncParams);
    //// For the purposes of calculating critical depth, the unsteady depth
    //// should be slighly less than the crown of the pipe?
    //m_ncParams.UnsteadyDepth = 0.95;
    //m_ncParamsInit = true;

    m_hpgCount = 0;
    m_currentHPG = -1;
}

bool IcapHpg::allocate(int count_in)
{
    m_hpgCount = count_in;
    return true;
}


std::shared_ptr<hpgns::BaseHPG> IcapHpg::getHpg(id_type linkId)
{
    if (m_list.count(linkId))
        return m_list[linkId];
    else
        return NULL;
}

bool IcapHpg::loadHPG(id_type linkId, const std::string& path)
{
    if (m_list.count(linkId))
        return false;

	int errCode = 0;
    //TODO:
	hpgns::BaseHPG* h = new hpgns::CircularHPG((char*)path.c_str());
    if (h == NULL)
        return false;
    else if (errCode = h->GetError())
        return false;

	m_list.insert(std::make_pair(linkId, std::shared_ptr<hpgns::BaseHPG>(h)));
	return true;
}

//
//bool IcapHpg::IsValidFlow(int linkId, double flow)
//{
//    std::shared_ptr<hpgns::BaseHPG> hpg = getHpg(linkId);
//    if (hpg == NULL)
//    {
//        setErrorMessage("Invalid HPG object");
//        return false;
//    }
//
//    return hpg->IsValidFlow(flow);
//}

//
//// Return -100 for error, -1 for pipe empty, 0 for valid flow/depth for
//// HPG, +1 for pipe full.
//int IcapHpg::CanInterpolateExtended(int linkId, double dsDepth, double flow)
//{
//    // Get the HPG.
//    std::shared_ptr<hpgns::BaseHPG> hpg = getHpg(linkId);
//    if (hpg == NULL)
//    {
//        setErrorMessage("Invalid HPG object");
//        return -1;
//    }
//
//    int okToContinue = 0;
//
//    double dsInvert = GLINK_DSINVERT(linkId);
//    double dsElev = dsDepth + dsInvert;
//    double diam = GLINK_MAXDEPTH(linkId);
//
//	int flowType = hpg->IsValidFlowExtended(flow);
//
//    // If the flow is greater than the max flow in the HPG, then return
//    // pipe full.
//    if (flowType > 0)
//        return HPG_PIPE_FULL;
//
//    // If the flow is lower than the minimum, then make it 105% of the minimum flow.
//    if (flowType < 0)
//        flow = getLowestFlow(linkId, flow) * 1.05;
//
//    // Get the flow indices of the curves that bracket this flow.
//    unsigned int curve1, curve2;
//    if (HPGFAILURE(hpg->GetFlowIndex(flow, curve1)))
//        return HPG_ERROR;
//    curve2 = curve1 + 1;
//
//    // We get the last points available for interpolation so we can
//    // tell if we are in the pressurized flow region.
//    hpgns::point lastP1, lastP2;
//
//    if (HPGFAILURE(hpg->GetLastPointOnCurve(flow, curve1, lastP1)))
//        return HPG_ERROR;
//    if (HPGFAILURE(hpg->GetLastPointOnCurve(flow, curve2, lastP2)))
//        return HPG_ERROR;
//
//	// If the depth is greater than the max allowed depth in the HPG, then
//	// return pipe full.
//	//if (dsDepth > diam * hpg->GetMaxDepth())
//    if (dsElev > lastP1.x || dsElev > lastP2.x)
//		return HPG_PIPE_FULL;
//
//    // Else, if the flow is lower than the minimum flow in the HPG, we
//    // then return pipe empty.
//    else if (flowType < 0)
//        return HPG_PIPE_EMPTY;
//
//    // Else, we probably have a valid elevation/flow combo to
//    // interpolate the upstream elevation for.
//    else
//        return HPG_PIPE_OK;
//}
//
//bool IcapHpg::CanInterpolate(int linkId, double dsDepth, double flow)
//{
//	return (CanInterpolateExtended(linkId, dsDepth, flow) == 0);
//}


bool IcapHpg::getUpstream(id_type linkId, var_type dsHead, var_type flow, var_type& usHead)
{
    std::shared_ptr<hpgns::BaseHPG> hpg = getHpg(linkId);
    if (hpg == NULL)
    {
        setErrorMessage("Invalid HPG object");
        return false;
    }

    double depth;
    int errCode = hpg->GetUpstream(flow, dsHead, usHead);

    if (HPGFAILURE(errCode))
    {
        char code[20];
        sprintf(code, "%d", errCode);
        setErrorMessage("hpg_getUpstream failed: code=" + std::string(code) + " message=" + hpg->GetErrorStr());
        return false;
    }
    else
    {
        return true;
    }
}


bool IcapHpg::getHf(id_type linkId, var_type dsHead, var_type flow, var_type& hf)
{
    std::shared_ptr<hpgns::BaseHPG> hpg = getHpg(linkId);
    if (hpg == NULL)
    {
        setErrorMessage("Invalid HPG object");
        return false;
    }

    int errCode = hpg->GetHf(flow, dsHead, hf);

    if (HPGFAILURE(errCode))
    {
        char code[20];
        sprintf(code, "%d", errCode);
        setErrorMessage("hpg_getHf failed: code=" + std::string(code) + " message=" + hpg->GetErrorStr());
        return false;
    }
    else
    {
        return true;
    }
}


var_type IcapHpg::getLowestFlow(id_type linkId, bool isAdverse)
{
    std::shared_ptr<hpgns::BaseHPG> hpg = getHpg(linkId);
    if (hpg == NULL)
    {
        setErrorMessage("Invalid HPG object");
    }
	else
	{
		if (isAdverse)
			return hpg->AdvFlowAt(0);
        else
			return hpg->PosFlowAt(0);
	}

	return 0.0;
}


bool IcapHpg::getVolume(id_type linkId, var_type dsHead, var_type flow, var_type& volume)
{
    std::shared_ptr<hpgns::BaseHPG> hpg = getHpg(linkId);
    if (hpg == NULL)
    {
        setErrorMessage("Invalid HPG object");
        return false;
    }

    int errCode = hpg->GetVolume(flow, dsHead, volume);

    if (HPGFAILURE(errCode))
    {
        char code[20];
        sprintf(code, "%d", errCode);
        setErrorMessage("hpg_getVolume failed: code=" + std::string(code) + " message=" + hpg->GetErrorStr());
        return false;
    }
	else
	    return true;
}

//
//bool IcapHpg::GetCritUpstream(int linkId, double flow, double& usDepth)
//{
//    std::shared_ptr<hpgns::BaseHPG> hpg = getHpg(linkId);
//    if (hpg == NULL)
//    {
//        setErrorMessage("Invalid HPG object");
//        return false;
//    }
//
//    double depth;
//    int errCode = hpg->GetCritUpstream(flow, depth);
//
//    if (HPGFAILURE(errCode))
//    {
//        setErrorMessage("hpg_getCritUpstream failed");
//        return false;
//    }
//    else
//    {
//        depth -= GLINK_USINVERT(linkId);
//        usDepth = depth;
//        return true;
//    }
//}
//
//
//bool IcapHpg::PressurizedInterpolation(int linkId, double dsDepth, double flow, double& usDepth_out)
//{
//    if (GLINK_TYPE(linkId) != LINKTYPE_CONDUIT)
//        return false;
//
//    // We set usDepth to be a "bad" value.
//    double usDepth = 0.0;
//
//    //TODOTAG: Check for units here.  We are assuming 1.486 is the K_n
//    // Manning's correction factor for English units. (1.0 for Metric.)
//
//    double roughness = GLINK_ROUGHNESS(linkId);
//    double length = GLINK_LENGTH(linkId);
//    double slope = GLINK_SLOPE(linkId);
//    double diameter = GLINK_MAXDEPTH(linkId);
//    double dsInvert = GLINK_DSINVERT(linkId);
//
//    std::shared_ptr<hpgns::BaseHPG> hpg = getHpg(linkId);
//    if (hpg == NULL)
//    {
//        setErrorMessage("Invalid HPG object");
//        return false;
//    }
//
//    // Compute the change in head delta-h.
//    double h_f = (roughness/1.486)*(roughness/1.486) *
//      flow*flow * length / (
//        (diameter*diameter*M_PI/4.0)*(diameter*diameter*M_PI/4.0) * 
//        pow(diameter/4.0, 4.0/3.0)
//      );
//
//    // We need to determine here if the HPG is "steep."  By this we mean that
//    // the last curve on the HPG is a steep-slope flow curve.  (This case
//    // doesn't include those HPGs that have a steep-flow region in the middle
//    // of the HPG, but end on a mild-flow curve.)
//    bool isSteep = false;
//    if (flow > 0.0)
//    {
//        unsigned int numFlows = hpg->NumPosFlows();
//        hpgns::point tempP = hpg->PosValuesAt(numFlows-1).front();
//        isSteep = (tempP.x > tempP.y ? true : false);
//    }
//
//    // If the HPG is "steep" and non-adverse, then we do some special
//    // computations to get the upstream water elevation.
//    if (isSteep)
//    {
//        double hCrit = 0.0;
//        bool critSuccess = CalculateCriticalDepth(flow, diameter, 0.01*diameter, hCrit);
//        //double yCrit = hCrit + GLINK_DSINVERT(linkId);
//
//        // If we couldn't compute critical depth, probably because the pipe is
//        // too full, we return downstream depth + head loss.
//        if (! critSuccess)
//            usDepth = dsDepth + h_f;
//
//        // If the pipe is definitely pressurized, then the downstream depth is
//        // modified to make sure it's either the critical depth or at least
//        // 50% of the pipe diameter (depending on the flow condition).  We then
//        // return this depth + head loss.
//        else if (IS_PRESSURIZED(dsDepth, diameter) || IS_PRESSURIZED(dsDepth + h_f, diameter) || IS_PRESSURIZED(hCrit, diameter))
//        {
//            if (hpg->IsValidFlow(flow))
//                usDepth = max(dsDepth, hCrit) + h_f;
//            else
//                usDepth = max(dsDepth, 0.5*diameter) + h_f;
//        }
//
//        // Otherwise, we approximate the upstream depth as critical.
//        else
//            usDepth = hCrit;
//    }//end steep case
//
//    // This case is for a normal "mild" flow or adverse-slope flow.
//    else
//    {
//        double yDnAt80Pct = PRESSURIZED_THRESHOLD * diameter;
//        double yUpAt80Pct = PRESSURIZED_THRESHOLD * diameter;
//        double yDnAt120Pct = 1.2 * diameter;
//        double yUpAt120Pct = yDnAt120Pct + h_f;
//
//        // If our downstream point is in the pressurized region, or our
//        // flow is completely pressurized, then we just use h_f.
//        if (! hpg->IsValidFlow(flow))
//        {
//            usDepth = dsDepth + h_f;
//        }
//        else if (dsDepth > yDnAt120Pct)
//        {
//            usDepth = dsDepth + h_f;
//        }
//
//        // If our downstream point is in the transitional region, then
//        // linearly interpolate.
//        else
//        {
//            // Get the last point on the curve for that flow.  This may not
//            // be equal to the 80% full point, so we want to be slightly more
//            // accurate.
//            hpgns::point fullPoint;
//            if (! HPGFAILURE(hpg->GetLastPoint(flow, fullPoint)))
//            {
//                yDnAt80Pct = fullPoint.x;
//                yUpAt80Pct = fullPoint.y;
//            }
//            else
//            {
//                setErrorMessage("Pressurized interpolation error in getting last poin on HPG curve");
//                return false;
//            }
//
//            double M = (yUpAt120Pct - yUpAt80Pct) / (yDnAt120Pct - yDnAt80Pct);
//            double B = yUpAt120Pct - M * yDnAt120Pct;
//            usDepth = M * dsDepth + B;
//        }
//    }//end non-steep case
//
//    // Adjust the depth to account for the change in height.
//    usDepth -= slope * length;
//    usDepth_out = usDepth;
//
//    return true;
//}
//
//
//int IcapHpg::GetHPGError(int linkId)
//{
//    std::shared_ptr<hpgns::BaseHPG> hpg = getHpg(linkId);
//    if (hpg == NULL)
//    {
//        setErrorMessage("Invalid HPG object");
//        return 1;
//    }
//
//    return hpg->GetError();
//}
//
//
//bool IcapHpg::CalculateCriticalDepth(double flow, double diameter, double initGuess, double& yCrit_out)
//{
//    if (!m_ncParamsInit)
//    {
//        return false;
//    }
//
//    m_ncParams.ErrorCode = 0;
//    double yCritical = ComputeCriticalDepth(flow, diameter, m_ncParams);
//    if (m_ncParams.ErrorCode)
//    {
//        return false;
//    }
//
//    yCrit_out = yCritical;
//    return true;
//
//    /*
//    int errorCode = 0;
//    // make an initial guess
//    double yCrit = initGuess;
//
//    // We are using a Newton-Raphson iterative approximation to the
//    // above mathematics.  We will iterate until the f(x) part of
//    // Newton-Raphson is less than the convergence factor, or we've
//    // iterated 40 times.
//
//    // set an initial (bad) function evaluation
//    double fEval = 100000.0;
//    int iterCount = 0;
//    int maxIter = 100;
//
//    // save this so we don't have to square 100 times
//    double diam2 = diameter * diameter;
//    double flow2 = flow * flow;
//    double maxDepth = PRESSURIZED_THRESHOLD * diameter;
//
//    bool finished = false;
//    while (iterCount < maxIter && !finished)
//    {
//        //TAG
//        // Note: this exits when defined range for acos [-1.0, 1.0] exceeded.
//        // Not sure if this is acceptable behavior.
//        double feedToAcos = 1.0 - 2.0 * yCrit / diameter;
//        if (feedToAcos < -1.0 || feedToAcos > 1.0)
//        {
//            errorCode = hpgns::error::imaginary;
//            break;
//        }
//
//        double theta = 2.0 * acos( feedToAcos );
//        double area = 1.0 / 8.0 * (theta - sin(theta)) * diam2; 
//        double topWidth = sin(theta / 2.0) * diameter; 
//
//        // calculate the function evaluation
//        fEval = 1.0 - ((topWidth * flow2) / (9.81 * area * area * area));
//
//        // calculate the function derivative evaluation
//        double dfEval = -flow2 / 9.81 * (-3.0 * topWidth * topWidth * pow(area, -4) + pow(area, -3) * (diameter - 2.0 * yCrit) * pow(yCrit, -0.5) * pow(diameter - yCrit, -0.5));
//
//        // calculate y_normal
//        yCrit = yCrit - fEval / dfEval;
//
//        if (fabs(fEval) < 1E-6 &&
//            yCrit < maxDepth)
//            finished = true;
//
//        // increment iteration count
//        iterCount++;
//    }
//
//    // If we've maxed out the number of iterations, then return non-converged flag
//    if (iterCount >= maxIter)
//    {
//        errorCode = hpgns::error::divergence;
//        hpg_setError("hpg_calculateCriticalDepth: Solution did not converge.");
//    }
//    else if (iterCount == 0)
//    {
//        errorCode = hpgns::error::at_max_depth;
//        hpg_setError("hpg_calculateCriticalDepth: Solution exceeded maximum depth.");
//    }
//
//    if (errorCode)
//    {
//        ICAP_HPG_Error = 1;
//        return false;
//    }
//    else
//    {
//        *yCrit_out = yCrit;
//        return true;
//    }
//    */
//}
//

int IcapHpg::loadNextHpg(const std::string& path, std::shared_ptr<geometry::LinkList> linkList)
{
    if (m_currentHPG < 0)
    {
        if (! allocate(linkList->count()))
            return 1;
    }

    m_currentHPG++;
    if (m_currentHPG >= linkList->count())
        return -1; // < 0 means done

    std::shared_ptr<geometry::Link> link = linkList->get(m_currentHPG);
    if (link->getGeometryType() != xs::xstype::circular)
        return 0;

    bool result = checkAndLoadHPG(link, path);

    if (result)
        return 0; // ok
    else
        return 1; // error
}


bool IcapHpg::loadHpgs(const std::string& path, std::shared_ptr<geometry::LinkList> linkList)
{
    bool result = allocate(linkList->count());
    if (! result)
        return false;
    
    bool ok = true;
    int count = linkList->count();
    for (int i = 0; i < count; i++)
    {
		printf("Loading HPG %d of %d...\n", i + 1, count);
        // We don't load the HPG if this isn't a conduit.
        //TODO:
        if (linkList->get(i)->getGeometryType() != xs::xstype::circular)
        {
            m_list[i] = NULL;
            continue;
        }

        if (! checkAndLoadHPG(linkList->get(i), path))
        {
            ok = false;
            break;
        }
    }

    return ok;
}


bool IcapHpg::checkAndLoadHPG(std::shared_ptr<geometry::Link> link, const std::string& dir)
{
    // Kludgy, I know, but HPG's can have two different file names:
    //   DT{ID}.txt  -- OR --   {ID}.txt
    // This loop checks for the existence of both.
    std::stringstream fileStream;

    fileStream << "DT" << link->getName() << ".txt";
    std::string file1 = fileStream.str();

    fileStream.str("");
    fileStream << link->getName() << ".txt";
    std::string file2 = fileStream.str();

    fileStream.str("");
    fileStream << dir << "\\" << file1;
    std::string hpgPath1 = fileStream.str();

    fileStream.str("");
    fileStream << dir << "\\" << file2;
    std::string hpgPath2 = fileStream.str();

    bool f1exists = boost::filesystem::exists(hpgPath1);
    bool f2exists = boost::filesystem::exists(hpgPath2);

    if (! f1exists && ! f2exists)
    {
        setErrorMessage("HPG not found.  File=" + hpgPath2);
        return false;
    }

    else if (f1exists && ! loadHPG(link->getId(), hpgPath1))
    {
        setErrorMessage("Failed to load HPG.  File=" + hpgPath1);
        return false;
    }

    else if (f2exists && ! loadHPG(link->getId(), hpgPath2))
    {
        setErrorMessage("Failed to load HPG.  File=" + hpgPath2);
        return false;
    }

    return true;
}

