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


#include <boost/algorithm/string.hpp>

#include "../util/parse.h"
#include "../xslib/circular.h"

#include "link.h"


namespace geometry
{

    Link::Link(const id_type& theId, const std::string& theName, std::shared_ptr<NodeFactory> theNodeFactory)
        : xs(xs::Factory::create(xs::xstype::dummy))
    { 
        this->name = theName;
        this->id = theId;
        this->nodeFactory = theNodeFactory;
        this->dsInvert = this->usInvert = 0;
    }

    var_type& Link::variable(variables::Variables var)
    {
        return this->simData[var];
    }

    bool Link::parseLine(const std::vector<std::string>& parts)
    {
        using namespace std;

        if (parts.size() < 7 || !this->nodeFactory)
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

        this->xs = std::unique_ptr<xs::CrossSection>(xs::Factory::create(parts[1]));
        //this->xs =  std::unique_ptr<xs::CrossSection>(new xs::Circular());// xs::Factory::create(parts[1]));
        //this->xs = xs::Factory::create(parts[1]);
        vector<string>::const_iterator it = parts.begin();
        it++;
        it++;
        
        if (!this->xs->setParameters(it, parts.end()))
        {
            setErrorMessage(this->xs->getErrorMessage());
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

    void Link::computeInvertsFromNodes()
    {
        this->dsInvert = this->outletNode->getInvert() + this->dsOffset;
        this->usInvert = this->inletNode->getInvert() + this->usOffset;
        this->slope = (this->usInvert - this->dsInvert) / this->length;
    }

    var_type Link::computeLevelVolume(var_type dsDepth)
    {
        double y2 = dsDepth - this->slope * this->length; // upstream
        double L = length;
        double S = slope;
	    double dx = (L / 100.0);
	    double xi = 0.0;
	    double V = 0.0;
	    double yi, a1, a2;

	    while (xi < L)
	    {
		    yi = (-S * (xi - L) + y2);
		    a1 = this->xs->computeArea(yi);
		
		    xi += dx;
		    yi = (-S * (xi - L) + y2);
		    a2 = this->xs->computeArea(yi);

		    V += dx * (a1 + a2) / 2.0;
	    }

	    return V;
    }

    void Link::propogateFlow(double upstreamInflow)
    {
        variable(variables::LinkFlow) += upstreamInflow;
        this->outletNode->propogateFlowDownstream(upstreamInflow);
    }

    void Link::resetFlow()
    {
        variable(variables::LinkFlow) = 0;
    }
    
    void Link::resetDepth()
    {
    }
}

