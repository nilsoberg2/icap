#define _CRT_SECURE_NO_DEPRECATE

#include "icap.h"

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif


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


bool ICAP::computeNodeLosses(int nodeIdx, ICAPNode* node)
{
    // At this point we are in cases where we need to compute losses.

    // Don't compute losses if the there is only one pipe.
    int degree = (int)node->linkIdx.size();
    if (degree < 2)
        return true;

    // Next we get all of the IDs of the downstream and upstream
    // nodes.  The main branch (mainId) is just the first ID in the
    // node list for this junction.

    int downIdx, mainIdx, latIdx;
    downIdx = mainIdx = latIdx = INVALID_IDX;

    for (int i = 0; i < degree; i++)
    {
        int linkIdx = node->linkIdx.at(i);
        if (GLINK_USNODE(linkIdx) == nodeIdx)
            downIdx = linkIdx;
        else if ((mainIdx == INVALID_IDX && GLINK_FLOW(linkIdx) > 0.0) ||
                 latIdx != INVALID_IDX)
            mainIdx = linkIdx;
        else
            latIdx = linkIdx;
    }

    // If we couldn't find a down index, return true because we can't
    // compute losses.
    if (downIdx == INVALID_IDX)
        return true;

    // If no "main" was found but a "lateral" branch was, then swap them.
    if (mainIdx == INVALID_IDX && latIdx != INVALID_IDX)
    {
        mainIdx = latIdx;
        latIdx = INVALID_IDX;
    }

    // If neither "main" nor "lateral" branches were found, then return an error.
    else if (mainIdx == INVALID_IDX && latIdx == INVALID_IDX)
        return false;

    // Work in floats because that's what the junction loss code computes
    // things in.

    // Get the downstream pipe's depth and diameter.
    float yDown = (float)GLINK_USDEPTH(downIdx);
    float dDown = (float)GLINK_MAXDEPTH(downIdx);

    // Get the main branch flow and diameter.  In the case that
    // the main branch is a structure (dropshaft) we assume that
    // the diameter of the main branch is the same as the diameter
    // of the downstream pipe.
    float qMain = (float)GLINK_FLOW(mainIdx);
    float dMain;
    if (GLINK_TYPE(mainIdx) == LINKTYPE_CONDUIT && GLINK_GEOMTYPE(mainIdx) == XSGEOM_CIRCULAR)
        dMain = (float)GLINK_MAXDEPTH(mainIdx);
    else
        dMain = dDown;

    // Next get the lateral branch flow and diameter.  These are
    // zero if there is no lateral branch.  In the case that the
    // lateral branch is a structure we assume that the diameter
    // of the lateral branch is the same as the diameter of the
    // downstream pipe.
    float qLat = 0.0;
    float dLat = 0.0;
    if (latIdx != INVALID_IDX)
    {
        qLat = (float)GLINK_FLOW(latIdx);
        if (GLINK_TYPE(latIdx) == LINKTYPE_CONDUIT && GLINK_GEOMTYPE(latIdx) == XSGEOM_CIRCULAR)
            dLat = (float)GLINK_MAXDEPTH(latIdx);
        else
            dLat = dDown;
    }

    // Next determine the angle.  It is zero if there is no lateral branch.
    double angleDbl = 0.0;
    if (latIdx != INVALID_IDX)
    {
        if (! computeAngle(downIdx, mainIdx, latIdx, &angleDbl))
            return false;
    }
    float angle = (float)angleDbl;

    // Now we compute the loss
    float grav = 32.174f;
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
        if (qMain == 0.0 && qLat != 0.0)
            yMain = yLat;

        // Else if the lateral branch doesn't have flow, and the
        // main branch has flow, then we carry the main water depth
        // over to the lateral branch.
        else if (qMain != 0.0 && qLat == 0.0)
            yLat = yMain;
    }

    // Finally we can save the new water depths.
    for (unsigned int i = 0; i < node->linkIdx.size(); i++)
    {
        int linkIdx = node->linkIdx.at(i);
        if (linkIdx == mainIdx)
            updateUpstreamDepthForNode(linkIdx, fabs(yMain));
        else if (linkIdx == latIdx)
            updateUpstreamDepthForNode(linkIdx, fabs(yLat));
        else
            updateUpstreamDepthForNode(linkIdx, GNODE_DEPTH(nodeIdx));
    }

    return true;
}


bool ICAP::computeAngle(int downIdx, int mainIdx, int latIdx, double* angle)
{

    // First check if junction locations are even included in the input file.
    // If they're not, then we assume that the lateral angle is 90 deg.
    if (!m_hasJunctionCoords)
    {
        *angle = 90.0;
        return true;
    }


    //ICAPLink* down = m_network.FindLink(downIdx);
    //if (down == NULL)
    //    return false;

    ICAPLink* main = m_network.FindLink(mainIdx);
    if (main == NULL)
    {
        char temp[25];
        sprintf(temp, "%d", mainIdx);
        m_errorStr += "Unable to find main link ";
        m_errorStr += temp;
        m_errorStr += " in computeAngle.";
        return false;
    }

    ICAPLink* lat = m_network.FindLink(latIdx);
    if (lat == NULL)
    {
        char temp[25];
        sprintf(temp, "%d", latIdx);
        m_errorStr += "Unable to find lateral link ";
        m_errorStr += temp;
        m_errorStr += " in computeAngle.";
        return false;
    }

    // Link[downIdx].n1 is equal to Link[mainIdx].n2 and is also equal to
    // Link[latIdx].n2

    double xd = main->x2,
           yd = main->y2;
    
    double xm, ym;
    if (main->verts.size() > 0)
    {
        xm = main->verts.back().x;
        ym = main->verts.back().y;
    }
    else
    {
        xm = main->x1;
        ym = main->y1;
    }

    double xl, yl;
    if (lat->verts.size() > 0)
    {
        xl = lat->verts.back().x;
        yl = lat->verts.back().y;
    }
    else
    {
        xl = lat->x1;
        yl = lat->y1;
    }

    double dp = (xd - xm) * (xd - xl) + (yd - ym) * (yd - yl);
    double cp = (xd - xm) * (yd - yl) - (xd - xl) * (yd - ym);

    // Compute the angle in radians and then convert to degrees.
    *angle = fabs(atan2(cp, dp) * 180.0 / M_PI);

    return true;
}

