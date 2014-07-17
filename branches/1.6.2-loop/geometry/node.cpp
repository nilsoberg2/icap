
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#include "../util/parse.h"
#include "../util/math.h"
#include "../model/units.h"

#include "node.h"
#include "link.h"


namespace geometry
{
    Node::Node(const id_type& theId, const std::string& theName, NodeType theType)
        : id(theId), name(theName), nodeType(theType)
    {
        this->xCoord = 0;
        this->yCoord = 0;
    }

    Node::~Node()
    {
        clearInflowObjects();
    }

    std::shared_ptr<Inflow> Node::getInflow()
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

    void Node::startInflow(DateTime dateTime)
    {
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

    void Node::clearInflowObjects()
    {
        this->inflows.clear();
    }

    void Node::attachInflow(std::shared_ptr<Inflow> inflow)
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


    const std::vector<std::shared_ptr<Link>>& Node::getUpstreamLinks() const
    {
        return this->usLinks;
    }


    const std::vector<std::shared_ptr<Link>>& Node::getDownstreamLinks() const
    {
        return this->dsLinks;
    }

    
    var_type Node::getDownstreamLinkMaxDepth()
    {
        var_type maxDepth = -1;
        for (auto link: this->dsLinks)
        {
            maxDepth = std::max(link->getMaxDepth(), maxDepth);
        }

        return maxDepth;
    }


    var_type Node::computeUpstreamLinksAngle(int downIdx, int mainIdx, int lateralIdx)
    {
        if (this->usLinks.size() < 2)
        {
            return var_type(0);
        }

        var_type xd = this->xCoord;
        var_type yd = this->yCoord;

        const std::vector<std::pair<var_type, var_type>>& vertices = this->usLinks[mainIdx]->getVertices();

        var_type xm = this->usLinks[mainIdx]->getUpstreamNode()->getXCoord();
        var_type ym = this->usLinks[mainIdx]->getUpstreamNode()->getYCoord();
        if (vertices.size() > 0)
        {
            xm = vertices.back().first;
            ym = vertices.back().second;
        }
        
        const std::vector<std::pair<var_type, var_type>>& vertices2 = this->usLinks[lateralIdx]->getVertices();

        var_type xl = this->usLinks[lateralIdx]->getUpstreamNode()->getXCoord();
        var_type yl = this->usLinks[lateralIdx]->getUpstreamNode()->getYCoord();
        if (vertices2.size() > 0)
        {
            xm = vertices2.back().first;
            ym = vertices2.back().second;
        }

        var_type dp = (xd - xm) * (xd - xl) + (yd - ym) * (yd - yl);
        var_type cp = (xd - xm) * (yd - yl) - (xd - xl) * (yd - ym);

        // Compute the angle in radians and then convert to degrees.
        return fabs(atan2(cp, dp) * 180.0 / M_PI);
    }


    //void Node::propagateDepthUpstream(double depth)
    //{
    //    bool diameterChanges = false;
    //    bool hasDownstream = false;
    //    double downDiam = 0;
    //    for (int i = 0; i < this->dsLinks.size(); i++)
    //    {
    //        if (this->dsLinks[i]->getGeometryType() != xs::xstype::dummy)
    //        {
    //            downDiam = std::max(downDiam, this->dsLinks[i]->getMaxDepth());
    //            hasDownstream = true;
    //        }
    //    }

    //    for (int i = 0; i < this->usLinks.size(); i++)
    //    {
    //        this->usLinks[i]->setDownstreamDepth(depth);
    //        if (this->usLinks[i]->getGeometryType() != xs::xstype::dummy && downDiam > 0 && fabs(this->usLinks[i]->getMaxDepth() - downDiam) > 0.1)
    //        {
    //            diameterChanges = true;
    //        }
    //    }

    //    //TODO:
    //    //if (hasDownstream && !this->usLinks.empty() && !isZero(variable(variables::NodeFlow)) && (this->usLinks.size() + this->dsLinks.size() != 2 || diameterChanges))
    //    //    adjustForJunctionLoss();
    //}
}

