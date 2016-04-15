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


#include <algorithm>
#include <boost/filesystem.hpp>

#include "../util/parse.h"

#include "icap.h"


id_type ICAP::findFirstNode(geometry::NodeType sinkNodeType)
{
    id_type nodeIdx = INVALID_IDX;
    
    int i = 0;
    for (auto iter = m_geometry->beginNode(); iter != m_geometry->endNode(); iter++)
    {
        if (iter->second->getType() == sinkNodeType)
        {
            nodeIdx = i;
            break;
        }
        i++;
    }

    return nodeIdx;
}


bool ICAP::validateGeometry()
{
    // Now find the sink node.
    m_sinkNodeIdx = findFirstNode(geometry::NodeType_Storage);

    if (m_sinkNodeIdx == INVALID_IDX)
    {
        setErrorMessage("Failed to find a STORAGE or OUTFALL node");
        return false;
    }

    return true;
}
//
//
//int ICAP::findSources(SourceList& sourceList)
//{
//    
//    for (auto iter = m_geometry->beginNode(); iter != m_geometry->endNode(); iter++)
//    {
//        Source temp;
//        temp.link = 0;
//        temp.active = false;
//
//        if (iter->second->hasInflowObjects())
//        {
//            temp.node = iter->first;
//            temp.active = true;
//        }
//
//        if (temp.active)
//        {
//            sourceList.AddSource(temp);
//        }
//    }
//
//	// Add reservoir as a source
//	Source resSource;
//	resSource.link = INVALID_IDX;
//	resSource.active = true;
//	int tempNode = INVALID_IDX;
//	findSinkLink(STORAGE, tempNode);
//	if (tempNode != INVALID_IDX && GNODE_HASINFLOW(tempNode))
//	{
//		resSource.node = tempNode;
//		sourceList.AddSource(resSource);
//	}
//
//    return sourceList.Count();
//}
//
