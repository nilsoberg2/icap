#ifndef LINK_H__
#define LINK_H__

#include <string>
#include <vector>
#include <map>
#include "nodeFactory.h"
#include "parseable.h"
#include "../model/modelElement.h"


namespace geometry
{

    namespace link
    {
        enum Geometry
        {
            Dummy,
            Circular,
            Irregular,
        };
    }

    class ICAP_API Link : public IModelElement, public Parseable
    {
    private:

        INodeFactory* nodeFactory;
        IModel* theModel;

        std::map<variables::Variables, double> simData;

    protected:
    public:
        double dsOffset;
        double usOffset;
        double maxDepth;
        double length;
        double roughness;
        std::string id;
        link::Geometry geomType;
        Node* inletNode;
        Node* outletNode;
        std::vector<std::pair<double, double>> vertices;

        double flow;


        Node* getUpstreamNode() { return this->inletNode; }
        Node* getDownstreamNode() { return this->outletNode; }


        void propogateFlow(double upstreamInflow);

        Link(const std::string& idValue, INodeFactory* theNodeFactory, IModel* model);

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
        virtual void propagateDepthUpstream(double depth) {};
        //TODO:
        void setDownstreamDepth(double) { };



        virtual void setErrorMessage(const std::string& msg) { Parseable::setErrorMessage("[" + this->id + "] " + msg); }
        virtual void appendErrorMessage(const std::string& msg) { Parseable::appendErrorMessage("[" + this->id + "] " + msg); }
    };

}

#endif//LINK_H__


