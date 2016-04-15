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

#include "storage.h"


namespace geometry
{
    StorageUnit::StorageUnit(const id_type& theId, const std::string& theName, std::shared_ptr<CurveFactory> factory)
        : Node(theId, theName, NodeType::NodeType_Storage)
    {
        this->curveFactory = factory;
        this->storageCurve = NULL;
    }

    var_type StorageUnit::lookupVolume(var_type depth)
    {
        if (this->storageCurve == NULL)
        {
            return this->funcCoeff * std::pow(depth, this->funcExp) + this->funcConst;
        }
        else
        {
            return this->storageCurve->integrateUpTo(depth);
        }
    }

    bool StorageUnit::parseLine(const std::vector<std::string>& parts)
    {
        using namespace std;
        using namespace boost::algorithm;

        if (parts.size() < 6)
        {
            setErrorMessage("At least 6 parts are required for STORAGE line");
            return false;
        }

        if (!baseParseLine(parts))
        {
            return false;
        }

        if (to_upper_copy(parts[4]) == "FUNCTIONAL")
        {
            if (parts.size() < 8)
            {
                setErrorMessage("At least 8 parts are required for FUNCTIONAL curve");
                return false;
            }

            if (!tryParse(parts[5], this->funcCoeff))
            {
                setErrorMessage("Unable to parse A for FUNCTIONAL curve");
                return false;
            }

            if (!tryParse(parts[6], this->funcExp))
            {
                setErrorMessage("Unable to parse B for FUNCTIONAL curve");
                return false;
            }

            if (!tryParse(parts[7], this->funcConst))
            {
                setErrorMessage("Unable to parse C for FUNCTIONAL curve");
                return false;
            }
        }
        else
        {
            if (this->curveFactory == NULL)
            {
                setErrorMessage("No curve factory given");
                return false;
            }

            this->storageCurve = this->curveFactory->getOrCreateCurve(parts[5]);
        }
    }

}
