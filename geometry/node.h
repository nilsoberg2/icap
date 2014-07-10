#ifndef NODE_H__
#define NODE_H__

#include <string>
#include <vector>
#include <map>
#include "inflow.h"
#include "parseable.h"
#include "../model/modelElement.h"


namespace geometry
{

    enum NodeType
    {
        NodeType_Junction,
        NodeType_Storage,
    };

    class Link;

    class ICAP_API Node : public IModelElement, public Parseable
    {
    private:
        std::vector<Inflow*> inflows;

        std::vector<Link*> usLinks;
        std::vector<Link*> dsLinks;

        double computeLateralInflow(DateTime dateTime);

        IModel* theModel;

        std::map<variables::Variables, double> simData;

    protected:
        Node(const std::string& theId, IModel* model);

        bool baseParseLine(const std::vector<std::string>& parts);

    public:
        double invertElev;
        double maxDepth;
        double initDepth;
        std::string id;
        NodeType nodeType;
        double xCoord, yCoord;

        /// <summary>
        /// The total inflow to this node: all upstream conduit inflows as well
        /// as any lateral inflows.
        /// </summary>
        double nodeInflow;

        /// <summary>
        /// The lateral inflow to this node only; the sum of all inflows from external
        /// inflows for the given time ste.p    
        /// </summary>
        double lateralInflow;
        double depth;
        double volume;
        bool canFlood;

    public:
        ~Node();

        typedef std::vector<Link*>::iterator LinkIter;

        LinkIter beginUpstreamLink();
        LinkIter endUpstreamLink();

        double getXCoord() { return this->xCoord; }
        double getYCoord() { return this->yCoord; }

        /// <summary>
        /// Returns 0 if there are not enough upstream links; returns the largest angle between
        /// any two links.
        /// </summary>
        double getUpstreamLinksAngle();

        /// <summary>
        /// Add an upstream conduit to this node's connectivity.
        /// </summary>
        void addUpstreamLink(Link* link) { this->usLinks.push_back(link); }

        /// <summary>
        /// Add a downstream conduit to this node's connectivity.
        /// </summary>
        void addDownstreamLink(Link* link) { this->dsLinks.push_back(link); }
        
        /// <summary>
        /// Add an external inflow object to this node.
        /// </summary>
        void attachInflow(Inflow* inflow);

        /// <summary>
        /// Modifies any attached inflow scale factors to the given factor.
        /// </summary>
        void setInflowFactor(double flowFactor);
        
        /// <summary>
        /// This function takes any external inflows at this node and propogates them downstream
        /// in a steady-state fashion.
        /// </summary>
        void startInflow();
        
        /// <summary>
        /// This function takes an upstream pipe inflow and propogates it downstream to the next
        /// pipe(s).
        /// </summary>
        void propogateFlowDownstream(double flow);
        
        /// <summary>
        /// This function resets the inflow parameters (e.g. at the start of a timestep).
        /// </summary>
        virtual void resetFlow();

        /// <summary>
        /// This function resets the depth parameter.
        /// </summary>
        virtual void resetDepth();
        
        /// <summary>
        /// Parse a given line (in the form of a vector of individual line parts) and store
        /// the parameters into this current object.
        /// </summary>
        virtual bool parseLine(const std::vector<std::string>& parts) = 0;
        
        /// <summary>
        /// Parse a coordinate line (in the form of a vector of individual line parts).
        /// </summary>
        bool parseCoordLine(const std::vector<std::string>& parts);
        
        virtual var_type& variable(variables::Variables var);

        virtual void propagateDepthUpstream(double depth);

        void adjustForJunctionLoss();

        Inflow* getInflow();



        virtual void setErrorMessage(const std::string& msg) { Parseable::setErrorMessage("[" + this->id + "] " + msg); }
        virtual void appendErrorMessage(const std::string& msg) { Parseable::appendErrorMessage("[" + this->id + "] " + msg); }
    };

}


#endif//NODE_H__
