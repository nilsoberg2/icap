
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
