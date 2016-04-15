// ==============================================================================
// ICAP License
// ==============================================================================
// University of Illinois/NCSA
// Open Source License
// 
// Copyright (c) 2014-2016 University of Illinois at Urbana-Champaign.
// All rights reserved.
// 
// Developed by:
// 
//     Nils Oberg
//     Blake J. Landry, PhD
//     Arthur R. Schmidt, PhD
//     Ven Te Chow Hydrosystems Lab
// 
//     University of Illinois at Urbana-Champaign
// 
//     https://vtchl.illinois.edu
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal with
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimers.
// 
//     * Redistributions in binary form must reproduce the above copyright notice,
//       this list of conditions and the following disclaimers in the
//       documentation and/or other materials provided with the distribution.
// 
//     * Neither the names of the Ven Te Chow Hydrosystems Lab, University of
// 	  Illinois at Urbana-Champaign, nor the names of its contributors may be
// 	  used to endorse or promote products derived from this Software without
// 	  specific prior written permission.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
// SOFTWARE.



#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>

#include "../model/modelElement.h"
#include "../model/units.h"
#include "../time/datetime.h"

#include "geometry.h"
#include "file_section.h"
#include "junction.h"
#include "storage.h"
#include "node.h"
#include "link.h"
#include "curve.h"
#include "timeseries.h"
#include "inflow.h"
#include "option.h"


namespace geometry
{
    class Geometry::Impl
    {
    public:
        //std::shared_ptr<Geometry> parent;
        Geometry* parent;

        std::map<std::string, FileSection> sectionNameMap;
        std::map<id_type, std::shared_ptr<Link>> linkMap; // Need to be deleted in dtor

        typedef std::map<id_type, std::shared_ptr<Node>>::iterator node_iter;
        std::map<id_type, std::shared_ptr<Node>> nodeMap; // Need to be deleted in dtor
        std::map<std::string, std::shared_ptr<Curve>> curveMap; // Need to be deleted in dtor
        std::map<std::string, std::shared_ptr<Timeseries>> tsMap; // Need to be deleted in dtor
        std::map<std::string, FileSection> objectTypeMapNode;
        std::map<std::string, FileSection> objectTypeMapLink;
        std::map<std::string, std::shared_ptr<Option>> options;

        std::map<std::string, id_type> nodeIdMap;
        std::map<std::string, id_type> linkIdMap;

        //std::vector<std::shared_ptr<Node>> sinkNodes;  // NOT needed to delete in dtor

        const std::vector<std::string>& getLinkIds() const;
        const std::vector<std::string>& getNodeIds() const;
        
        bool validateNetwork();
        bool validateOptions();
        
        bool loadFromSwmm5File(const std::string& filePath);
        bool scanFileForObjectTypes(const std::string& filePath);
        FileSection parseSectionLine(std::string line);
        bool parseDataLine(FileSection curSection, std::string line);

        //std::shared_ptr<Link> getOrCreateLink(std::string name);

        // Do not delete Inflow objects, that is taken care of by Node.
        std::shared_ptr<Inflow> createInflow();

        bool handleOption(std::shared_ptr<Option> option);

        std::string errorMsg;
    };



    
    ///////////////////////////////////////////////////////////////////////
    // NodeList interface

    int Geometry::node_count()
    {
        return impl->nodeMap.size();
    }

    std::shared_ptr<Node> Geometry::node_get(int index)
    {
        return impl->nodeMap[index];
    }

    NodeList* Geometry::getNodeList()
    {
        return this;
    }

    id_type Geometry::node_id(int index)
    {
        return index;
    }

    ///////////////////////////////////////////////////////////////////////
    // LinkList interface

    int Geometry::link_count()
    {
        return impl->linkMap.size();
    }

    std::shared_ptr<Link> Geometry::link_get(int index)
    {
        return impl->linkMap[index];
    }

    LinkList* Geometry::getLinkList()
    {
        return this;
    }

    id_type Geometry::link_id(int index)
    {
        return index;
    }


    

    ///////////////////////////////////////////////////////////////////////////
    // OPTIONS interface

    Options* Geometry::asOptions()
    {
        return this;
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
    




    Geometry::NodeIter Geometry::beginNode()
    {
        return impl->nodeMap.begin();
    }

    Geometry::NodeIter Geometry::endNode()
    {
        return impl->nodeMap.end();
    }

    Geometry::LinkIter Geometry::beginLink()
    {
        return impl->linkMap.begin();
    }

    Geometry::LinkIter Geometry::endLink()
    {
        return impl->linkMap.end();
    }


    
    DateTime Geometry::getStartDateTime()
    {
        return this->startDate + this->startTime;
    }

    DateTime Geometry::getEndDateTime()
    {
        return this->endDate + this->endTime;
    }
    

    Geometry::Geometry()
        : impl(new Impl())
    {
        populateSectionNameMap(impl->sectionNameMap);
        //impl->parent = shared_from_this(); // std::shared_ptr<Geometry>(this);
        impl->parent = this;
    }

    //template<typename T, typename T2>
    //void deleteFunction(std::pair<T, T2*> ptr)
    //{
    //    if (ptr.second != NULL)
    //    {
    //        delete ptr.second;
    //    }
    //}

    Geometry::~Geometry()
    {
        //std::for_each(impl->curveMap.begin(), impl->curveMap.end(), deleteFunction<std::string, Curve>);
        //std::for_each(impl->nodeMap.begin(), impl->nodeMap.end(), deleteFunction<id_type, Node>);
        //std::for_each(impl->linkMap.begin(), impl->linkMap.end(), deleteFunction<id_type, Link>);
        //std::for_each(impl->tsMap.begin(), impl->tsMap.end(), deleteFunction<std::string, Timeseries>);
        //std::for_each(impl->options.begin(), impl->options.begin(), deleteFunction<std::string, Option>);
        // Do not delete Inflow objects, that is taken care of by Node.
    }


    bool Geometry::loadFromFile(const std::string& filePath, GeometryFileFormat format)
    {
        if (!boost::filesystem::exists(filePath))
        {
            setErrorMessage(std::string("The file '") + filePath + "' does not exist.");
            return false;
        }

        this->geomFilePath = filePath;

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

                if (!this->processOptions())
                {
                    setErrorMessage("Failed to validate the options: " + getErrorMessage());
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


    void validateNetworkFunction(std::pair<id_type, std::shared_ptr<Link>> ptr)
    {
        std::shared_ptr<Node> dsNode = ptr.second->getDownstreamNode();
        dsNode->addUpstreamLink(ptr.second);

        std::shared_ptr<Node> usNode = ptr.second->getUpstreamNode();
        usNode->addDownstreamLink(ptr.second);

        ptr.second->computeInvertsFromNodes();
    }
    
    bool Geometry::Impl::validateNetwork()
    {
        // Add pointers to the links for each node.
        std::for_each(this->linkMap.begin(), this->linkMap.end(), validateNetworkFunction);

        //// Find all of the sink nodes (nodes with no outlets).
        //node_iter iter = this->nodeMap.begin();
        //while (iter != this->nodeMap.end())
        //{
        //    if (iter->second->beginUpstreamLink() == iter->second->endUpstreamLink())
        //    {
        //        this->sinkNodes.push_back(iter->second);
        //    }
        //    iter++;
        //}

        return true;
    }


    std::vector<std::string> Geometry::getLinkIds() const
    {
        std::vector<std::string> keys;
        for (auto imap: impl->linkMap)
        {
            keys.push_back(imap.second->getName());
        }
        std::sort(keys.begin(), keys.end());
        return keys;
    }
    

    std::vector<std::string> Geometry::getNodeIds() const
    {
        std::vector<std::string> keys;
        for (auto imap: impl->nodeMap)
        {
            keys.push_back(imap.second->getName());
        }
        std::sort(keys.begin(), keys.end());
        return keys;
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
                    if (curSection == FileSection::File_Conduit)
                    {
                        if (this->objectTypeMapLink.count(parts[0]) == 0)
                        {
                            this->objectTypeMapLink.insert(make_pair(parts[0], curSection));
                        }
                    }
                    else
                    {
                        if (this->objectTypeMapNode.count(parts[0]) == 0)
                        {
                            this->objectTypeMapNode.insert(make_pair(parts[0], curSection));
                        }
                    }
                }
            }
        }

        handle.close();

        return true;
    }


    bool Geometry::Impl::handleOption(std::shared_ptr<Option> option)
    {
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
            std::shared_ptr<Option> option = std::shared_ptr<Option>(new Option());
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
            if (parts[0] == "207")
            {
                double a = 1;
            }
            std::shared_ptr<Link> link = parent->getOrCreateLink(parts[0]);
            if (!link->parseLine(parts))
            {
                errorMsg = "Unable to parse link '" + parts[0] + "': " + link->getErrorMessage();
                return false;
            }
        }
        else if (curSection == FileSection::File_Xsection)
        {
            std::shared_ptr<Link> link = parent->getOrCreateLink(parts[0]);
            if (!link->parseXsection(parts))
            {
                errorMsg = "Unable to parse link cross section '" + parts[0] + "': " + link->getErrorMessage();
                return false;
            }
        }
        else if (curSection == FileSection::File_Junction || curSection == FileSection::File_Storage)
        {
            std::shared_ptr<Node> node = parent->getOrCreateNode(parts[0]);
            if (!node->parseLine(parts))
            {
                errorMsg = "Unable to parse node '" + parts[0] + "': " + node->getErrorMessage();
                return false;
            }
        }
        else if (curSection == FileSection::File_Curve)
        {
            std::shared_ptr<Curve> curve = parent->getOrCreateCurve(parts[0]);
            if (!curve->parseLine(parts))
            {
                errorMsg = "Unable to parse curve '" + parts[0] + "': " + curve->getErrorMessage();
                return false;
            }
        }
        else if (curSection == FileSection::File_Timeseries)
        {
            std::shared_ptr<Timeseries> ts = parent->getOrCreateTimeseries(parts[0]);
            if (!ts->parseLine(parts))
            {
                errorMsg = "Unable to parse timeseries '" + parts[0] + "': " + ts->getErrorMessage();
                return false;
            }
        }
        else if (curSection == FileSection::File_Inflow)
        {
            std::shared_ptr<Inflow> inflow = std::shared_ptr<Inflow>(new Inflow(std::shared_ptr<TimeseriesFactory>(this->parent)));
            if (!inflow->parseLine(parts))
            {
                errorMsg = "Unable to parse inflow '" + parts[0] + "': " + inflow->getErrorMessage();
                return false;
            }

            string nodeName = inflow->getInflowNodeName();
            if (nodeName.length() > 0)
            {
                std::shared_ptr<Node> node = parent->getOrCreateNode(nodeName);
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
            std::shared_ptr<Node> node = parent->getOrCreateNode(parts[0]);
            node->parseCoordLine(parts);
        }
        else if (curSection == FileSection::File_Vertex)
        {
            std::shared_ptr<Link> link = parent->getOrCreateLink(parts[0]);
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

    std::shared_ptr<Timeseries> Geometry::getOrCreateTimeseries(std::string tsName)
    {
        using namespace std;

        boost::algorithm::to_lower(tsName);

        if (impl->tsMap.count(tsName) > 0)
        {
            return impl->tsMap[tsName];
        }
        else
        {
            std::shared_ptr<Timeseries> ts = std::shared_ptr<Timeseries>(new Timeseries(tsName));
            ts->setStartDateTime(this->getStartDateTime());
            impl->tsMap.insert(make_pair(tsName, ts));
            return ts;
        }
    }

    std::shared_ptr<Curve> Geometry::getOrCreateCurve(std::string curveName)
    {
        using namespace std;

        boost::algorithm::to_lower(curveName);

        if (impl->curveMap.count(curveName) > 0)
        {
            return impl->curveMap[curveName];
        }
        else
        {
            std::shared_ptr<Curve> theCurve = std::shared_ptr<Curve>(new Curve(curveName));
            impl->curveMap.insert(make_pair(curveName, theCurve));
            return theCurve;
        }
    }

    std::shared_ptr<Node> Geometry::getOrCreateNode(std::string nodeId)
    {
        using namespace std;

        if (impl->nodeIdMap.count(nodeId) > 0)
        {
            return impl->nodeMap[impl->nodeIdMap[nodeId]];
        }
        else
        {
            std::shared_ptr<Node> theNode = NULL;
            if (impl->objectTypeMapNode[nodeId] == FileSection::File_Storage)
            {
                //theNode = std::shared_ptr<Node>(new StorageUnit(impl->nodeMap.size(), nodeId, std::shared_ptr<CurveFactory>(this)));
                theNode = std::shared_ptr<Node>(new StorageUnit(impl->nodeMap.size(), nodeId, shared_from_this()));
            }
            else
            {
                theNode = std::shared_ptr<Node>(new Junction(impl->nodeMap.size(), nodeId));
            }

            if (theNode != NULL)
            {
                impl->nodeIdMap.insert(make_pair(nodeId, impl->nodeMap.size()));
                impl->nodeMap.insert(make_pair(impl->nodeMap.size(), theNode));
            }

            return theNode;
        }
    }

    std::shared_ptr<Link> Geometry::getOrCreateLink(std::string linkId)
    {
        using namespace std;

        if (impl->linkIdMap.count(linkId) > 0)
        {
            return impl->linkMap[impl->linkIdMap[linkId]];
        }
        else
        {
            std::shared_ptr<Link> theLink = NULL;
            if (impl->objectTypeMapLink[linkId] == FileSection::File_Conduit)
            {
                /*theLink = std::shared_ptr<Link>(new Link(impl->linkMap.size(), linkId, dynamic_pointer_cast<NodeFactory>(this)));*/
                theLink = std::shared_ptr<Link>(new Link(impl->linkMap.size(), linkId, shared_from_this()));
            }

            if (theLink != NULL)
            {
                impl->linkIdMap.insert(make_pair(linkId, impl->linkMap.size()));
                impl->linkMap.insert(make_pair(impl->linkMap.size(), theLink));
            }

            return theLink;
        }
    }

    std::shared_ptr<Node> Geometry::getNode(std::string nodeId)
    {
        if (impl->nodeIdMap.count(nodeId) > 0)
        {
            return getNode(impl->nodeIdMap[nodeId]);
        }
        else
        {
            return NULL;
        }
    }

    std::shared_ptr<Node> Geometry::getNode(id_type nodeIdx)
    {
        if (impl->nodeMap.count(nodeIdx) > 0)
        {
            return impl->nodeMap[nodeIdx];
        }
        else
        {
            return NULL;
        }
    }
    
    std::shared_ptr<Link> Geometry::getLink(std::string linkId)
    {
        if (impl->linkIdMap.count(linkId) > 0)
        {
            return getLink(impl->linkIdMap[linkId]);
        }
        else
        {
            return NULL;
        }
    }

    std::shared_ptr<Link> Geometry::getLink(id_type linkIdx)
    {
        if (impl->linkMap.count(linkIdx) > 0)
        {
            return impl->linkMap[linkIdx];
        }
        else
        {
            return NULL;
        }
    }
}
