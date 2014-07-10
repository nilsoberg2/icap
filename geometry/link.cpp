
#include <boost/algorithm/string.hpp>
#include "link.h"
#include "parse.h"


namespace geometry
{

    Link::Link(const std::string& idValue, INodeFactory* theNodeFactory, IModel* model)
    { 
        this->theModel = model;
        this->id = idValue;
        this->nodeFactory = theNodeFactory;
    }

    var_type& Link::variable(variables::Variables var)
    {
        return this->simData[var];
    }

    bool Link::parseLine(const std::vector<std::string>& parts)
    {
        using namespace std;

        if (parts.size() < 7 || this->nodeFactory == NULL)
        {
            setErrorMessage("At least 7 parts are required for CONDUIT line");
            return false;
        }

        string inletNodeId = parts[1];
        string outletNodeId = parts[2];

        this->inletNode = this->nodeFactory->getOrCreateNode(inletNodeId);
        this->outletNode = this->nodeFactory->getOrCreateNode(outletNodeId);

        if (!tryParse(parts[3], this->length))
        {
            setErrorMessage("Unable to parse length");
            return false;
        }

        if (!tryParse(parts[4], this->roughness))
        {
            setErrorMessage("Unable to parse roughness");
            return false;
        }

        if (!tryParse(parts[5], this->usOffset))
        {
            setErrorMessage("Unable to parse US offset");
            return false;
        }

        if (!tryParse(parts[6], this->dsOffset))
        {
            setErrorMessage("Unable to parse DS offset");
            return false;
        }

        return true;
    }

    bool Link::parseXsection(const std::vector<std::string>& parts)
    {
        using namespace std;

        if (parts.size() < 3)
        {
            setErrorMessage("At least 3 parts are required for XSECTION line");
            return false;
        }

        string geom = boost::algorithm::to_lower_copy(parts[1]);
        this->geomType = link::Dummy;
        if (geom == "circular")
        {
            this->geomType = link::Circular;
        }
        else if (geom == "irregular")
        {
            this->geomType = link::Irregular;
        }

        if (!tryParse(parts[2], this->maxDepth))
        {
            setErrorMessage("Unable to parse max depth");
            return false;
        }

        return true;
    }

    bool Link::parseVertexLine(const std::vector<std::string>& parts)
    {
        if (parts.size() < 3)
        {
            setErrorMessage("At least 3 parts are required for VERTEX line");
            return false;
        }

        double x;
        if (!tryParse(parts[1], x))
        {
            setErrorMessage("Unable to parse X coordinate");
            return false;
        }

        double y;
        if (!tryParse(parts[2], y))
        {
            setErrorMessage("Unable to parse Y coordinate");
            return false;
        }

        this->vertices.push_back(std::pair<double, double>(x, y));

        return true;
    }

    void Link::propogateFlow(double upstreamInflow)
    {
        this->flow += upstreamInflow;
        this->outletNode->propogateFlowDownstream(this->flow);
    }

    void Link::resetFlow()
    {
        this->flow = 0;
    }
    
    void Link::resetDepth()
    {
    }
}

