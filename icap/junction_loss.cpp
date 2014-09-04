#define _CRT_SECURE_NO_DEPRECATE

#define _USE_MATH_DEFINES
#include <cmath>

#include "../model/units.h"
#include "../util/math.h"

#include "icap.h"


/*

Library JunctionLib contains a function that calculates the pressure losses in a wye junction using the equations given on pages 401-402 of Hager (1999).  This function uses the IMSL routine BCONF to minimize the sum of squares of errors between 'guessed' values for the upstream and lateral depths and those determined based on the downstream depth and calculated pressure-loss coefficients.  This optimization is bounded by critical depth at the lower end and the downstream depth plus the downstream diameter on the upper end.  If the best solution results in critical depth at one of the inflow branches this is flagged by a negative value for that depth.

The function returns the sum of squares of the error [ real(4 byte)]
The arguments are:
	Ydown	Real (4 byte)	Input	The downstream depth
	QLat	Real (4 byte)	Input	The flow in the lateral branch
	QMain	Real (4 byte)	Input	The flow in the main (upstream) branch
					QMain + QLat is the flow out downstream
	Dd	    Real (4 byte)	Input	The diameter of the downstream branch 
	Dm	    Real (4 byte) 	Input	The diameter of the main (upstream) branch
	Dl	    Real (4 byte) 	Input	The diamter of the lateral branch
	Angle	Real (4 byte)	Input	The angle between the main and lateral branches (degrees)
	Grav	Real (4 byte) 	Input	32.2 ft/sec^2 for Y & D in ft, Q in CFS
					9.81 m/sec^2 for Y and D in m, Q in CMS
	Yup	    Real (4 byte)	Output	The depth in the main (upstream branch)
	YLat	Real (4 byte)	Output	The depth in the lateral branch
	
The following is the visual basic declaration to use this library:

Declare Function CalcJunction Lib "JunctionLib.dll" _
   Alias "_CALCJUNCTION@40" (ByRef YDown As Single, _
   ByRef QLat As Single, ByRef QMain As Single, ByRef Dd As Single, _
   ByRef DMain As Single, ByRef DLat As Single, ByRef Angle As Single, _
   ByRef Grav As Single, ByRef Yup As Single, ByRef YLat As Single) As Single
   
   Public YDown As Single
   Public QLat As Single
   Public QMain As Single
   Public Dd As Single
   Public DMain As Single
   Public DLat As Single
   Public Angle As Single
   Public Grav As Single
   Public Yup As Single
   Public YLat As Single
   Public ErrSq As Single

The following is the actual visual basic call to this function:
    ErrSq = CalcJunction(YDown, QLat, QMain, Dd, DMain, DLat, Angle, Grav, Yup, YLat)
*/


extern "C" float __cdecl CALCJUNCTION(float* YDown, float* QLat, float* QMain, float* Dd,
                                      float* DMain, float* DLat, float* Angle, float* Grav,
                                      float* Yup, float* YLat);


bool ICAP::computeNodeLosses(const id_type& nodeId)
{
    std::shared_ptr<geometry::Node> node = m_geometry->getNode(nodeId);
    const std::vector<std::shared_ptr<geometry::Link>>& usLinks = node->getUpstreamLinks();
    const std::vector<std::shared_ptr<geometry::Link>>& dsLinks = node->getDownstreamLinks();

    // Don't compute losses if the there is only one pipe.
    if (dsLinks.size() < 1 || usLinks.size() < 1)
    {
        return true;
    }
        
    // Next we get all of the IDs of the downstream and upstream
    // nodes.  The main branch (mainId) is just the first ID in the
    // node list for this junction.

    int downIdx = 0;
    int mainIdx = INVALID_IDX, latIdx = INVALID_IDX;

    for (int i = 0; i < usLinks.size(); i++)
    {
        if ((mainIdx == INVALID_IDX && usLinks[i]->variable(variables::LinkFlow) > 0.0) || latIdx != INVALID_IDX)
            mainIdx = i;
        else
            latIdx = i;
    }
    
    // If no "main" was found but a "lateral" branch was, then swap them.
    if (mainIdx == INVALID_IDX && latIdx != INVALID_IDX)
    {
        mainIdx = latIdx;
        latIdx = INVALID_IDX;
    }

    // Get the downstream pipe's depth and max depth.
    float yDown = (float)dsLinks[downIdx]->variable(variables::LinkDsDepth);
    float dDown = (float)dsLinks[downIdx]->getMaxDepth();
        
    // Get the main branch flow and max depth.  In the case that
    // the main branch is a structure (dropshaft) we assume that
    // the max depth of the main branch is the same as the max depth
    // of the downstream pipe.
    float qMain = (float)usLinks[mainIdx]->variable(variables::LinkFlow);
    float dMain = (float)usLinks[mainIdx]->getMaxDepth();
    if (usLinks[mainIdx]->getGeometryType() == xs::xstype::dummy)
    {
        dMain = dDown;
    }

    // Next get the lateral branch flow and diameter.  These are
    // zero if there is no lateral branch.  In the case that the
    // lateral branch is a structure we assume that the diameter
    // of the lateral branch is the same as the diameter of the
    // downstream pipe.
    float qLat = 0;
    float dLat = 0;
    if (latIdx != INVALID_IDX)
    {
        qLat = usLinks[latIdx]->variable(variables::LinkFlow);
        dLat = usLinks[latIdx]->getMaxDepth();
        if (usLinks[latIdx]->getGeometryType() == xs::xstype::dummy)
        {
            dLat = dDown;
        }
    }
        
    // Next determine the angle.  It is zero if there is no lateral branch.
    float angle = (float)node->computeUpstreamLinksAngle(downIdx, mainIdx, latIdx);
    if (isZero(angle))
    {
        BOOST_LOG_SEV(m_log, loglevel::debug) << "Unable to calculate junction losses due to zero angle";
        return false;
    }
        
    // Now we compute the loss
    float grav = (float)UCS->g();
    float yMain = 0.0f;
    float yLat = 0.0f;
    float err = CALCJUNCTION(&yDown, &qLat, &qMain, &dDown, &dMain, &dLat, &angle, &grav, &yMain, &yLat);
        
    // Now, if there is a lateral branch and no flow in one of
    // the branches, then we need to carry the water depths from
    // the branch that has flow to the branch that doesn't.
    if (latIdx != INVALID_IDX)
    {
        // If there the main branch doesn't have flow and the lateral
        // branch has flow, then we carry the lateral water depth
        // over to the main branch.
        if (isZero(qMain) && !isZero(qLat))
        {
            yMain = yLat;
        }

        // Else if the lateral branch doesn't have flow, and the
        // main branch has flow, then we carry the main water depth
        // over to the lateral branch.
        else if (!isZero(qMain) && isZero(qLat))
        {
            yLat = yMain;
        }

        usLinks[latIdx]->variable(variables::LinkDsDepth) = yLat;
    }

    usLinks[mainIdx]->variable(variables::LinkDsDepth) = yMain;

    return true;
}
