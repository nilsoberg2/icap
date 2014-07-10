#ifndef GEOMETRY_H__
#define GEOMETRY_H__

#include <string>
#include <map>
#include <memory>

#include "fileSection.h"
#include "curveFactory.h"
#include "nodeFactory.h"
#include "timeseriesFactory.h"
#include "node.h"
#include "link.h"
#include "curve.h"
#include "timeseries.h"
#include "inflow.h"
#include "../model/model.h"
#include "../api.h"
#include "../time/datetime.h"


namespace geometry
{

    enum GeometryFileFormat
    {
        FileFormatSwmm5
    };


    class ICAP_API Geometry : public ICurveFactory, public INodeFactory, public ITimeseriesFactory, public IModel
    {
    private:
        class Impl;
        std::unique_ptr<Impl> impl;

        std::string errorMsg;
        void setErrorMessage(const std::string& message) { this->errorMsg = message; }

    public:
        Geometry();
        ~Geometry();

        bool loadFromFile(const std::string& filePath, GeometryFileFormat format);



        Node* findNode(std::string nodeId);
        Link* findLink(std::string linkId);

        // Iteration methods.
        typedef std::map<std::string, Node*>::iterator NodeIter;
        typedef std::map<std::string, Link*>::iterator LinkIter;

        NodeIter beginNode();
        NodeIter endNode();


        // Interface methods
        virtual Curve* getOrCreateCurve(std::string curveName);
        virtual Node* getOrCreateNode(std::string nodeId);
        virtual Timeseries* getOrCreateTimeseries(std::string tsId);


        double getDownstreamLinkMaxDepth(const std::string& nodeId);

        ///////////////////////////////////////////////////////////////////////
        // IModel interface method.

        virtual void resetTimestep(DateTime dateTime);
        virtual void startTimestep(DateTime dateTime);
        virtual DateTime getCurrentDateTime();
        virtual void resetDepths();
        virtual void addRealTimeInput(std::string nodeId);
        virtual void setRealTimeInputFlow(std::string nodeId, double flow);
        virtual double getRealTimeNodeHead(std::string nodeId);
        
        virtual var_type getNodeVariable(std::string nodeId, variables::Variables var);
        virtual var_type getLinkVariable(std::string linkId, variables::Variables var);

        /// <summary>
        /// This function sets the given depth for the given node ID and then moves
        /// that depth to any upstream conduits.  Node depths can be different than
        /// upstream conduit depths because of transition losses in junctions or geometry changes.
        /// </summary>
        virtual bool propagateNodeDepth(std::string nodeId, double depth);



        std::string getErrorMessage() { return this->errorMsg; }
        
        std::vector<std::string> getLinkIds() const;
        std::vector<std::string> getNodeIds() const;
        
        std::vector<std::string> getOptionNames() const;
        std::string getOption(std::string optionName) const;
        bool hasOption(std::string optionName) const;
        DateTime getStartDateTime();
        DateTime getEndDateTime();
    };

}

#endif//GEOMETRY_H__
