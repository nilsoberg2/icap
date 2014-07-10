
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include "node.h"
#include "link.h"
#include "parse.h"
#include "../util/math.h"
#include "../model/units.h"
#include "../calcJunction.h"


namespace geometry
{
    Node::Node(const std::string& theId, IModel* model)
    {
        this->id = theId;
        this->xCoord = 0;
        this->yCoord = 0;
        this->theModel = model;
    }

    Node::~Node()
    {
        for (int i = 0; i < this->inflows.size(); i++)
        {
            if (this->inflows[i] != NULL)
                delete this->inflows[i];
        }
    }

    Inflow* Node::getInflow()
    {
        if (this->inflows.size() > 0)
        {
            return this->inflows[0];
        }
        else
        {
            return NULL;
        }
    }
    
    Node::LinkIter Node::beginUpstreamLink()
    {
        return this->usLinks.begin();
    }

    Node::LinkIter Node::endUpstreamLink()
    {
        return this->usLinks.end();
    }
    
    var_type& Node::variable(variables::Variables var)
    {
        return this->simData[var];
    }

    void Node::startInflow()
    {
        DateTime dateTime = this->theModel->getCurrentDateTime();
        this->lateralInflow = computeLateralInflow(dateTime);
        this->nodeInflow += this->lateralInflow;

        for (int i = 0; i < this->dsLinks.size(); i++)
        {
            this->dsLinks[i]->propogateFlow(this->lateralInflow);
        }
    }

    void Node::propogateFlowDownstream(double flow)
    {
        this->nodeInflow += flow;
        for (int i = 0; i < this->dsLinks.size(); i++)
        {
            this->dsLinks[i]->propogateFlow(flow);
        }
    }

    double Node::computeLateralInflow(DateTime dateTime)
    {
        double flowRate = 0;
        for (int i = 0; i < this->inflows.size(); i++)
        {
            if (this->inflows[i] != NULL)
                flowRate += this->inflows[i]->getInflow(dateTime);
        }
        return flowRate;
    }

    void Node::resetFlow()
    {
        this->nodeInflow = 0;
        this->lateralInflow = 0;
    }

    void Node::resetDepth()
    {
        this->depth = 0;
    }

    void Node::attachInflow(Inflow* inflow)
    {
        this->inflows.push_back(inflow);
    }

    void Node::setInflowFactor(double flowFactor)
    {
        for (int i = 0; i < this->inflows.size(); i++)
        {
            this->inflows[i]->setInflowFactor(flowFactor);
        }
    }

    bool Node::baseParseLine(const std::vector<std::string>& parts)
    {
        if (parts.size() < 2)
        {
            setErrorMessage("At least 2 parts are required for JUNCTION line");
            return false;
        }

        if (!tryParse(parts[1], this->invertElev))
        {
            setErrorMessage("Unable to parse invert elevation");
            return false;
        }

        if (parts.size() > 2 && !tryParse(parts[2], this->maxDepth))
        {
            setErrorMessage("Unable to prase max depth");
            return false;
        }

        if (parts.size() > 3 && !tryParse(parts[3], this->initDepth))
        {
            setErrorMessage("Unable to parse init depth");
            return false;
        }

        return true;
    }

    bool Node::parseCoordLine(const std::vector<std::string>& parts)
    {
        if (parts.size() < 3)
        {
            setErrorMessage("At least 3 parts are required for COORDINATE line");
            return false;
        }

        if (!tryParse(parts[1], this->xCoord))
        {
            setErrorMessage("Unable to parse X coordinate");
            return false;
        }

        if (!tryParse(parts[2], this->yCoord))
        {
            setErrorMessage("Unable to parse Y coordinate");
            return false;
        }

        return true;
    }

    double Node::getUpstreamLinksAngle()
    {
        if (this->usLinks.size() < 2)
        {
            return 0;
        }

        double xd = this->xCoord;
        double yd = this->yCoord;

        double xm = this->usLinks[0]->getUpstreamNode()->getXCoord();
        double ym = this->usLinks[0]->getUpstreamNode()->getYCoord();
        if (this->usLinks[0]->vertices.size() > 0)
        {
            xm = this->usLinks[0]->vertices.back().first;
            ym = this->usLinks[0]->vertices.back().second;
        }

        double xl = this->usLinks[1]->getUpstreamNode()->getXCoord();
        double yl = this->usLinks[1]->getUpstreamNode()->getYCoord();
        if (this->usLinks[1]->vertices.size() > 0)
        {
            xm = this->usLinks[1]->vertices.back().first;
            ym = this->usLinks[1]->vertices.back().second;
        }

        double dp = (xd - xm) * (xd - xl) + (yd - ym) * (yd - yl);
        double cp = (xd - xm) * (yd - yl) - (xd - xl) * (yd - ym);

        // Compute the angle in radians and then convert to degrees.
        return fabs(atan2(cp, dp) * 180.0 / M_PI);
    }

    void Node::propagateDepthUpstream(double depth)
    {
        bool diameterChanges = false;
        bool hasDownstream = false;
        double downDiam = 0;
        for (int i = 0; i < this->dsLinks.size(); i++)
        {
            if (this->dsLinks[i]->geomType != link::Dummy)
            {
                downDiam = std::max(downDiam, this->dsLinks[i]->maxDepth);
                hasDownstream = true;
            }
        }

        for (int i = 0; i < this->usLinks.size(); i++)
        {
            this->usLinks[i]->setDownstreamDepth(depth);
            if (this->usLinks[i]->geomType != link::Dummy && downDiam > 0 && fabs(this->usLinks[i]->maxDepth - downDiam) > 0.1)
            {
                diameterChanges = true;
            }
        }

        if (hasDownstream && !this->usLinks.empty() && !isZero(variable(variables::NodeFlow)) && (this->usLinks.size() + this->dsLinks.size() != 2 || diameterChanges))
            adjustForJunctionLoss();
    }

    void Node::adjustForJunctionLoss()
    {
        // Don't compute losses if the there is only one pipe.
        if (this->dsLinks.size() + this->usLinks.size() < 2)
            return;
        
        // Next we get all of the IDs of the downstream and upstream
        // nodes.  The main branch (mainId) is just the first ID in the
        // node list for this junction.

        float yDown = this->dsLinks[0]->variable(variables::LinkDsDepth);
        float dDown = this->dsLinks[0]->maxDepth;
        
        // Get the main branch flow and diameter.  In the case that
        // the main branch is a structure (dropshaft) we assume that
        // the diameter of the main branch is the same as the diameter
        // of the downstream pipe.
        float qMain = this->usLinks[0]->variable(variables::LinkFlow);
        float dMain = this->usLinks[0]->maxDepth;
        
        // Next get the lateral branch flow and diameter.  These are
        // zero if there is no lateral branch.  In the case that the
        // lateral branch is a structure we assume that the diameter
        // of the lateral branch is the same as the diameter of the
        // downstream pipe.
        float qLat = 0;
        float dLat = 0;
        if (this->usLinks.size() > 1)
        {
            qLat = this->usLinks[1]->variable(variables::LinkFlow);
            dLat = this->usLinks[1]->maxDepth;
        }
        
        // Next determine the angle.  It is zero if there is no lateral branch.
        float angle = getUpstreamLinksAngle();
        
        // Now we compute the loss
        float grav = UCS->g();
        float yMain = 0.0f;
        float yLat = 0.0f;
        float err = CALCJUNCTION(&yDown, &qLat, &qMain, &dDown, &dMain, &dLat, &angle, &grav, &yMain, &yLat);
        
        // Now, if there is a lateral branch and no flow in one of
        // the branches, then we need to carry the water depths from
        // the branch that has flow to the branch that doesn't.
        if (this->usLinks.size() > 1)
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

            this->usLinks[1]->variable(variables::LinkDsDepth) = yLat;
        }

        this->usLinks[0]->variable(variables::LinkDsDepth) = yMain;
    }
}

