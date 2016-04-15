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


	/// The Geometry class represents a parsed pipe network.
    class Geometry : public CurveFactory, public NodeFactory, public TimeseriesFactory,
        public NodeList_helper, public LinkList_helper, public Parseable, public Options,
        public std::enable_shared_from_this<Geometry>
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
        std::string geomFilePath;

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
        NodeList* getNodeList();

        ///////////////////////////////////////////////////////////////////////
        // LinkList interface
        virtual int link_count();
        virtual std::shared_ptr<Link> link_get(int index);
        virtual id_type link_id(int index);
        LinkList* getLinkList();

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
        virtual Options* asOptions();
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

    private:
        std::shared_ptr<Link> Geometry::getOrCreateLink(std::string linkId);
    };

}

#endif//GEOMETRY_H__
