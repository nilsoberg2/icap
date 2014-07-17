#ifndef GEOMETRY_H__
#define GEOMETRY_H__

#include <string>
#include <map>
#include <memory>

#include "../model/model.h"
#include "../api.h"
#include "../time/datetime.h"

#include "file_section.h"
#include "curve_factory.h"
#include "node_factory.h"
#include "timeseries_factory.h"
#include "node.h"
#include "link.h"
#include "curve.h"
#include "timeseries.h"
#include "inflow.h"
#include "node_list.h"
#include "link_list.h"
#include "options.h"


namespace geometry
{

    enum GeometryFileFormat
    {
        FileFormatSwmm5
    };


    class Geometry : public CurveFactory, public NodeFactory, public TimeseriesFactory,
        public NodeList_helper, public LinkList_helper, public Parseable, public Options
    {
    private:
        class Impl;
        std::unique_ptr<Impl> impl;
        
    protected:
        virtual bool processOptions() { return true; }
        
        DateTime startDate;
        DateTime startTime;
        DateTime endDate;
        DateTime endTime;

    public:

        ///////////////////////////////////////////////////////////////////////
        // Constructors and loading methods
        Geometry();
        ~Geometry();
        bool loadFromFile(const std::string& filePath, GeometryFileFormat format);

        ///////////////////////////////////////////////////////////////////////
        // NodeList interface
        virtual int node_count();
        virtual std::shared_ptr<Node> node_get(int index);
        virtual id_type node_id(int index);
        std::shared_ptr<NodeList> getNodeList();

        ///////////////////////////////////////////////////////////////////////
        // LinkList interface
        virtual int link_count();
        virtual std::shared_ptr<Link> link_get(int index);
        virtual id_type link_id(int index);
        std::shared_ptr<LinkList> getLinkList();

        ///////////////////////////////////////////////////////////////////////
        // CurveFactory interface
        virtual std::shared_ptr<Curve> getOrCreateCurve(std::string curveName);
        
        ///////////////////////////////////////////////////////////////////////
        // NodeFactory interface
        virtual std::shared_ptr<Node> getOrCreateNode(std::string nodeId);
        
        ///////////////////////////////////////////////////////////////////////
        // TimeseriesFactory interface
        virtual std::shared_ptr<Timeseries> getOrCreateTimeseries(std::string tsId);

        ///////////////////////////////////////////////////////////////////////
        // Error/option/time methods
        std::vector<std::string> getLinkIds() const;
        std::vector<std::string> getNodeIds() const;
        virtual std::vector<std::string> getOptionNames() const;
        virtual std::string getOption(std::string optionName) const;
        virtual bool hasOption(std::string optionName) const;
        virtual std::shared_ptr<Options> asOptions();
        DateTime getStartDateTime();
        DateTime getEndDateTime();
        
        ///////////////////////////////////////////////////////////////////////
        // Node/link getters.
        std::shared_ptr<Node> getNode(std::string nodeId);
        std::shared_ptr<Node> getNode(id_type nodeId);
        std::shared_ptr<Link> getLink(std::string linkId);
        std::shared_ptr<Link> getLink(id_type linkId);
        
        ///////////////////////////////////////////////////////////////////////
        // Iterators for accessing all nodes.
        typedef std::map<id_type, std::shared_ptr<Node>>::iterator NodeIter;
        NodeIter beginNode();
        NodeIter endNode();
        typedef std::map<id_type, std::shared_ptr<Link>>::iterator LinkIter;
        LinkIter beginLink();
        LinkIter endLink();
    };

}

#endif//GEOMETRY_H__
