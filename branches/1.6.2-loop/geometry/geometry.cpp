

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>

#include "geometry.h"
#include "fileSection.h"
#include "junction.h"
#include "storage.h"
#include "node.h"
#include "link.h"
#include "curve.h"
#include "timeseries.h"
#include "inflow.h"
#include "../model/modelElement.h"
#include "rtInflow.h"
//#include "../common.h"
#include "../model/units.h"
#include "option.h"
#include "../time/datetime.h"


Units* UCS = new Units(UnitSystem::Units_US);



namespace geometry
{
    class Geometry::Impl
    {
    public:
        Geometry* parent;

        std::map<std::string, FileSection> sectionNameMap;
        std::map<std::string, Link*> linkMap; // Need to be deleted in dtor

        typedef std::map<std::string, Node*>::iterator node_iter;
        std::map<std::string, Node*> nodeMap; // Need to be deleted in dtor
        std::map<std::string, Curve*> curveMap; // Need to be deleted in dtor
        std::map<std::string, Timeseries*> tsMap; // Need to be deleted in dtor
        std::map<std::string, RealTimeInflow*> rtInflowMap; // DO NOT delete in dtor
        std::map<std::string, FileSection> objectTypeMap;
        std::map<std::string, Option*> options;

        std::vector<Node*> sinkNodes;  // NOT needed to delete in dtor

        DateTime startDate;
        DateTime startTime;
        DateTime endDate;
        DateTime endTime;

        DateTime currentDateTime;

        const std::vector<std::string>& getLinkIds() const;
        const std::vector<std::string>& getNodeIds() const;
        
        bool validateNetwork();
        bool validateOptions();
        
        bool loadFromSwmm5File(const std::string& filePath);
        bool scanFileForObjectTypes(const std::string& filePath);
        FileSection parseSectionLine(std::string line);
        bool parseDataLine(FileSection curSection, std::string line);

        Link* getOrCreateLink(std::string id);

        // Do not delete Inflow objects, that is taken care of by Node.
        Inflow* createInflow();

        bool handleOption(Option* option);

        std::string errorMsg;
    };


    Geometry::NodeIter Geometry::beginNode()
    {
        return impl->nodeMap.begin();
    }

    Geometry::NodeIter Geometry::endNode()
    {
        return impl->nodeMap.end();
    }


    Geometry::Geometry()
        : impl(new Impl())
    {
        populateSectionNameMap(impl->sectionNameMap);
        impl->parent = this;
    }

    template<typename T>
    void deleteFunction(std::pair<std::string, T*> ptr)
    {
        if (ptr.second != NULL)
        {
            delete ptr.second;
        }
    }

    Geometry::~Geometry()
    {
        std::for_each(impl->curveMap.begin(), impl->curveMap.end(), deleteFunction<Curve>);
        std::for_each(impl->nodeMap.begin(), impl->nodeMap.end(), deleteFunction<Node>);
        std::for_each(impl->linkMap.begin(), impl->linkMap.end(), deleteFunction<Link>);
        std::for_each(impl->tsMap.begin(), impl->tsMap.end(), deleteFunction<Timeseries>);
        std::for_each(impl->options.begin(), impl->options.begin(), deleteFunction<Option>);
        // Do not delete Inflow objects, that is taken care of by Node.
    }


    bool Geometry::loadFromFile(const std::string& filePath, GeometryFileFormat format)
    {
        if (!boost::filesystem::exists(filePath))
        {
            setErrorMessage(std::string("The file '") + filePath + "' does not exist.");
            return false;
        }

        if (format == GeometryFileFormat::FileFormatSwmm5)
        {
            bool result = impl->loadFromSwmm5File(filePath);
            if (!result)
            {
                setErrorMessage(std::string("Unable to parse file '") + filePath + "': " + impl->errorMsg);
                return false;
            }
            else
            {
                if (!impl->validateNetwork())
                {
                    setErrorMessage("Failed to validate the network");
                    return false;
                }

                if (!impl->validateOptions())
                {
                    setErrorMessage("Failed to validate the options: " + impl->errorMsg);
                    return false;
                }

                return true;
            }
        }
        else
        {
            setErrorMessage("Unsupported file format provided");
            return false;
        }
    }


    void validateNetworkFunction(std::pair<std::string, Link*> ptr)
    {
        Node* dsNode = ptr.second->outletNode;//getDownstreamNode();
        dsNode->addUpstreamLink(ptr.second);

        Node* usNode = ptr.second->inletNode;//getUpstreamNode();
        usNode->addDownstreamLink(ptr.second);
    }
    
    bool Geometry::Impl::validateNetwork()
    {
        // Add pointers to the links for each node.
        std::for_each(this->linkMap.begin(), this->linkMap.end(), validateNetworkFunction);

        // Find all of the sink nodes (nodes with no outlets).
        node_iter iter = this->nodeMap.begin();
        while (iter != this->nodeMap.end())
        {
            if (iter->second->beginUpstreamLink() == iter->second->endUpstreamLink())
            {
                this->sinkNodes.push_back(iter->second);
            }
            iter++;
        }

        return true;
    }

    
    bool Geometry::Impl::validateOptions()
    {
        if (this->options.count("start_date") == 0 || this->options.count("start_time") == 0 ||
            this->options.count("end_date") == 0 || this->options.count("end_time") == 0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }


    std::vector<std::string> Geometry::getLinkIds() const
    {
        std::vector<std::string> keys;
        for (auto imap: impl->linkMap)
        {
            keys.push_back(imap.first);
        }
        std::sort(keys.begin(), keys.end());
        return keys;
    }
    
    std::vector<std::string> Geometry::getNodeIds() const
    {
        std::vector<std::string> keys;
        for (auto imap: impl->nodeMap)
        {
            keys.push_back(imap.first);
        }
        std::sort(keys.begin(), keys.end());
        return keys;
    }

    std::vector<std::string> Geometry::getOptionNames() const
    {
        std::vector<std::string> keys;
        for (auto imap: impl->options)
        {
            keys.push_back(imap.first);
        }
        std::sort(keys.begin(), keys.end());
        return keys;
    }

    bool Geometry::hasOption(std::string option) const
    {
        return impl->options.count(option) > 0;
    }

    std::string Geometry::getOption(std::string option) const
    {
        if (impl->options.count(option))
        {
            return impl->options[option]->getValue();
        }
        else
        {
            return "";
        }
    }

    DateTime Geometry::getStartDateTime()
    {
        return impl->startDate + impl->startTime;
    }

    DateTime Geometry::getEndDateTime()
    {
        return impl->endDate + impl->endTime;
    }


    bool Geometry::Impl::loadFromSwmm5File(const std::string& filePath)
    {
        if (!scanFileForObjectTypes(filePath))
        {
            return false;
        }

        using namespace std;
        ifstream handle(filePath);
        if (!handle.good())
        {
            errorMsg = "Unable to open the input file";
            return false;
        }

        FileSection curSection = FileSection::File_None;

        string line;
        while (getline(handle, line).good())
        {
            boost::trim(line);
            boost::algorithm::to_lower(line);

            if (line.length() == 0 || line[0] == ';')
            {
                continue;
            }

            if (line[0] == '[')
            {
                curSection = parseSectionLine(line);
            }
            else if (curSection != FileSection::File_None)
            {
                if (!parseDataLine(curSection, line))
                {
                    return false;
                }
            }
        }
        

        handle.close();

        return true;
    }
    

    bool Geometry::Impl::scanFileForObjectTypes(const std::string& filePath)
    {
        using namespace std;

        ifstream handle(filePath);

        if (!handle.good())
        {
            errorMsg = "Unable to open the input file";
            return false;
        }

        FileSection curSection = FileSection::File_None;

        string line;
        while (getline(handle, line).good())
        {
            boost::trim(line);
            boost::algorithm::to_lower(line);

            if (line.length() == 0 || line[0] == ';')
            {
                continue;
            }

            if (line[0] == '[')
            {
                curSection = parseSectionLine(line);
            }
            else if (curSection == FileSection::File_Conduit ||
                curSection == FileSection::File_Junction ||
                curSection == FileSection::File_Storage)
            {
                vector<string> parts;
                boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" \t"), boost::algorithm::token_compress_on);

                if (parts.size() > 1)
                {
                    if (this->objectTypeMap.count(parts[0]) == 0)
                    {
                        this->objectTypeMap.insert(make_pair(parts[0], curSection));
                    }
                }
            }
        }

        handle.close();

        return true;
    }


    bool Geometry::Impl::handleOption(Option* option)
    {
        if (option->getName() == "start_date")
        {
            if (!DateTime::tryParseDate(option->getValue().c_str(), this->startDate, Format::M_D_Y))
            {
                this->errorMsg = "Unable to parse option " + option->toString();
                return false;
            }
            else
            {
                return true;
            }
        }

        if (option->getName() == "start_time")
        {
            if (!DateTime::tryParseTime(option->getValue().c_str(), this->startTime))
            {
                this->errorMsg = "Unable to parse option " + option->toString();
                return false;
            }
            else
            {
                return true;
            }
        }
        
        if (option->getName() == "end_date")
        {
            if (!DateTime::tryParseDate(option->getValue().c_str(), this->endDate, Format::M_D_Y))
            {
                this->errorMsg = "Unable to parse option " + option->toString();
                return false;
            }
            else
            {
                return true;
            }
        }

        if (option->getName() == "end_time")
        {
            if (!DateTime::tryParseTime(option->getValue().c_str(), this->startTime))
            {
                this->errorMsg = "Unable to parse option " + option->toString();
                return false;
            }
            else
            {
                return true;
            }
        }

        return true;
    }

    bool Geometry::Impl::parseDataLine(FileSection curSection, std::string line)
    {
        using namespace std;
        using namespace boost::algorithm;

        trim(line);

        if (line.length() == 0 || line[0] == ';')
        {
            return true;
        }

        vector<string> parts;
        split(parts, line, is_any_of(" \t"), token_compress_on);

        if (parts.size() == 0)
        {
            return true;
        }

        // Clean up quotation marks around any parameters
        for (int i = 0; i < parts.size(); i++)
        {
            trim_if(parts[i], is_any_of("\""));
        }

        if (curSection == FileSection::File_Option)
        {
            Option* option = new Option();
            if (!option->parseLine(line))
            {
                errorMsg = "Unable to parse option '" + parts[0] + "': " + option->getErrorMessage();
                return false;
            }
            handleOption(option);
            this->options.insert(make_pair(option->getName(), option));
        }
        else if (curSection == FileSection::File_Conduit)
        {
            Link* link = getOrCreateLink(parts[0]);
            if (!link->parseLine(parts))
            {
                errorMsg = "Unable to parse link '" + parts[0] + "': " + link->getErrorMessage();
                return false;
            }
        }
        else if (curSection == FileSection::File_Xsection)
        {
            Link* link = getOrCreateLink(parts[0]);
            if (!link->parseXsection(parts))
            {
                errorMsg = "Unable to parse link cross section '" + parts[0] + "': " + link->getErrorMessage();
                return false;
            }
        }
        else if (curSection == FileSection::File_Junction || curSection == FileSection::File_Storage)
        {
            Node* node = parent->getOrCreateNode(parts[0]);
            if (!node->parseLine(parts))
            {
                errorMsg = "Unable to parse node '" + parts[0] + "': " + node->getErrorMessage();
                return false;
            }
        }
        else if (curSection == FileSection::File_Curve)
        {
            Curve* curve = parent->getOrCreateCurve(parts[0]);
            if (!curve->parseLine(parts))
            {
                errorMsg = "Unable to parse curve '" + parts[0] + "': " + curve->getErrorMessage();
                return false;
            }
        }
        else if (curSection == FileSection::File_Timeseries)
        {
            Timeseries* ts = parent->getOrCreateTimeseries(parts[0]);
            if (!ts->parseLine(parts))
            {
                errorMsg = "Unable to parse timeseries '" + parts[0] + "': " + ts->getErrorMessage();
                return false;
            }
        }
        else if (curSection == FileSection::File_Inflow)
        {
            Inflow* inflow = new Inflow(this->parent);
            if (!inflow->parseLine(parts))
            {
                errorMsg = "Unable to parse inflow '" + parts[0] + "': " + inflow->getErrorMessage();
                return false;
            }

            string nodeName = inflow->getInflowNodeName();
            if (nodeName.length() > 0)
            {
                Node* node = parent->getOrCreateNode(nodeName);
                node->attachInflow(inflow);
            }
            else
            {
                errorMsg = "Unable to parse inflow '" + parts[0] + "': no inflow node provided";
                return false;
            }
        }
        else if (curSection == FileSection::File_Coordinate)
        {
            Node* node = parent->getOrCreateNode(parts[0]);
            node->parseCoordLine(parts);
        }
        else if (curSection == FileSection::File_Vertex)
        {
            Link* link = getOrCreateLink(parts[0]);
            link->parseVertexLine(parts);
        }

        return true;
    }


    FileSection Geometry::Impl::parseSectionLine(std::string line)
    {
        using namespace std;

        if (line[0] != '[' || line[line.size() - 1] != ']')
        {
            return FileSection::File_None;
        }

        std::string kwd = line.substr(1, line.size() - 2);

        if (this->sectionNameMap.count(kwd) == 1)
        {
            return this->sectionNameMap[kwd];
        }
        else
        {
            return FileSection::File_None;
        }
    }

    Timeseries* Geometry::getOrCreateTimeseries(std::string tsName)
    {
        using namespace std;

        boost::algorithm::to_lower(tsName);

        if (impl->tsMap.count(tsName) > 0)
        {
            return impl->tsMap[tsName];
        }
        else
        {
            Timeseries* ts = new Timeseries(tsName);
            ts->setStartDateTime(this->getStartDateTime());
            impl->tsMap.insert(make_pair(tsName, ts));
            return ts;
        }
    }

    Curve* Geometry::getOrCreateCurve(std::string curveName)
    {
        using namespace std;

        boost::algorithm::to_lower(curveName);

        if (impl->curveMap.count(curveName) > 0)
        {
            return impl->curveMap[curveName];
        }
        else
        {
            Curve* theCurve = new Curve(curveName);
            impl->curveMap.insert(make_pair(curveName, theCurve));
            return theCurve;
        }
    }

    Node* Geometry::getOrCreateNode(std::string nodeId)
    {
        using namespace std;

        if (impl->nodeMap.count(nodeId) > 0)
        {
            return impl->nodeMap[nodeId];
        }
        else
        {
            Node* theNode = NULL;
            if (impl->objectTypeMap[nodeId] == FileSection::File_Storage)
            {
                theNode = new StorageUnit(nodeId, this, this);
            }
            else
            {
                theNode = new Junction(nodeId, this);
            }

            if (theNode != NULL)
            {
                impl->nodeMap.insert(make_pair(nodeId, theNode));
            }

            return theNode;
        }
    }

    Link* Geometry::Impl::getOrCreateLink(std::string linkId)
    {
        using namespace std;

        if (this->linkMap.count(linkId) > 0)
        {
            return this->linkMap[linkId];
        }
        else
        {
            Link* theLink = NULL;
            if (this->objectTypeMap[linkId] == FileSection::File_Conduit)
            {
                theLink = new Link(linkId, this->parent, this->parent);
            }

            if (theLink != NULL)
            {
                this->linkMap.insert(make_pair(linkId, theLink));
            }

            return theLink;
        }
    }

    void resetFlowFunction(std::pair<std::string, IModelElement*> ptr)
    {
        ptr.second->resetFlow();
    }

    void Geometry::resetTimestep(DateTime dateTime)
    {
        std::for_each(impl->nodeMap.begin(), impl->nodeMap.end(), resetFlowFunction);
        std::for_each(impl->linkMap.begin(), impl->linkMap.end(), resetFlowFunction);
    }

    void propFlowFunction(std::pair<std::string, Node*> ptr)
    {
        ptr.second->startInflow();
    }

    void Geometry::startTimestep(DateTime dateTime)
    {
        impl->currentDateTime = dateTime;
        std::for_each(impl->nodeMap.begin(), impl->nodeMap.end(), propFlowFunction);
    }

    void Geometry::addRealTimeInput(std::string nodeId)
    {
        boost::algorithm::to_lower(nodeId);
        Node* node = findNode(nodeId);
        if (node == NULL)
        {
            return;
        }

        RealTimeInflow* inflow = new RealTimeInflow();

        node->attachInflow(inflow);

        impl->rtInflowMap.insert(std::pair<std::string, RealTimeInflow*>(nodeId, inflow));
    }

    void Geometry::setRealTimeInputFlow(std::string nodeId, double flow)
    {
        boost::algorithm::to_lower(nodeId);
        if (impl->rtInflowMap.count(nodeId) == 0)
        {
            return;
        }

        impl->rtInflowMap[nodeId]->setCurrentInflow(flow);
    }

    double Geometry::getRealTimeNodeHead(std::string nodeId)
    {
        Node* node = findNode(nodeId);
        if (node == NULL)
        {
            return ERROR_VAL;
        }

        return node->depth + node->invertElev;
    }

    var_type Geometry::getNodeVariable(std::string nodeId, variables::Variables var)
    {
        Node* node = findNode(nodeId);
        if (node == NULL)
        {
            return variables::error_val;
        }

        return node->variable(var);
    }

    var_type Geometry::getLinkVariable(std::string linkId, variables::Variables var)
    {
        Link* link = findLink(linkId);
        if (link == NULL)
        {
            return variables::error_val;
        }

        return link->variable(var);
    }

    double Geometry::getDownstreamLinkMaxDepth(const std::string& nodeId)
    {
        Node* node = findNode(nodeId);
        if (node == NULL)
        {
            return variables::error_val;
        }

        //return node->getDownstreamLinkMaxDepth(
    }

    bool Geometry::propagateNodeDepth(std::string nodeId, double depth)
    {
        Node* node = findNode(nodeId);
        if (node == NULL)
        {
            return false;
        }

        node->propagateDepthUpstream(depth);

        return true;
    }

    DateTime Geometry::getCurrentDateTime()
    {
        //TODO:
        return DateTime();
    }

    void Geometry::resetDepths()
    {
        //TODO:
    }

    Node* Geometry::findNode(std::string nodeId)
    {
        if (impl->nodeMap.count(nodeId) > 0)
        {
            return impl->nodeMap[nodeId];
        }
        else
        {
            return NULL;
        }
    }

    Link* Geometry::findLink(std::string linkId)
    {
        if (impl->linkMap.count(linkId) > 0)
        {
            return impl->linkMap[linkId];
        }
        else
        {
            return NULL;
        }
    }
}
