#ifndef LINK_H__
#define LINK_H__

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "../util/parseable.h"
#include "../model/modelElement.h"
#include "../xslib/cross_section.h"

#include "node_factory.h"


namespace geometry
{

    class Link : public IModelElement, public Parseable
    {
    protected:

        std::shared_ptr<NodeFactory> nodeFactory;
        
        std::string name;
        id_type id;

        std::map<variables::Variables, var_type> simData;
        
        var_type dsInvert;
        var_type usInvert;
        var_type dsOffset;
        var_type usOffset;
        var_type maxDepth;
        var_type length;
        var_type roughness;
        var_type slope;
        std::shared_ptr<Node> inletNode;
        std::shared_ptr<Node> outletNode;
        std::vector<std::pair<var_type, var_type>> vertices;

        std::shared_ptr<xs::CrossSection> xs;
        double flow;
        
    public:
        id_type getId() { return this->id; }
        std::string getName() { return this->name; }
        std::shared_ptr<Node> getUpstreamNode() { return this->inletNode; }
        std::shared_ptr<Node> getDownstreamNode() { return this->outletNode; }
        var_type getMaxDepth() { return this->maxDepth; }
        xs::xstype getGeometryType() { return this->xs->getType(); }
        var_type getDownstreamInvert() { return this->dsInvert; }
        var_type getUpstreamInvert() { return this->usInvert; }
        var_type getSlope() { return this->slope; }
        var_type getLength() { return this->length; }

        const std::vector<std::pair<var_type, var_type>>& getVertices() const { return this->vertices; }

        void propogateFlow(var_type upstreamInflow);

        Link(const id_type& theId, const std::string& theName, std::shared_ptr<NodeFactory> theNodeFactory);

        bool parseLine(const std::vector<std::string>& parts);
        bool parseXsection(const std::vector<std::string>& parts);
        bool parseVertexLine(const std::vector<std::string>& parts);

        /// <summary>
        /// This function resets the inflow parameters (e.g. at the start of a timestep).
        /// </summary>
        virtual void resetFlow();

        /// <summary>
        /// This function resets the depth parameter.
        /// </summary>
        virtual void resetDepth();

        virtual var_type& variable(variables::Variables var);

        
        //TODO:
        virtual void propagateDepthUpstream(var_type depth) {};
        //TODO:
        void setDownstreamDepth(var_type) { };

        /// <summary>
        /// Compute the invert from the offsets and node inverts and also compute the slope.
        /// </summary>
        void computeInvertsFromNodes();

        var_type computeLevelVolume(var_type dsDepth);

        virtual void setErrorMessage(const std::string& msg) { Parseable::setErrorMessage("[" + this->name + "] " + msg); }
        virtual void appendErrorMessage(const std::string& msg) { Parseable::appendErrorMessage("[" + this->name + "] " + msg); }

        //friend class Node;
    };

}

#endif//LINK_H__


