
#include "icap.h"
#include <algorithm>


int ICAP::findSinkLink(int sinkNodeType, int& sinkNodeIdx)
{
    int subIndex = INVALID_IDX;
    int nodeIdx = ICAP::findFirstNode(sinkNodeType, subIndex);

    if (subIndex == INVALID_IDX || nodeIdx == INVALID_IDX)
        return INVALID_IDX;

    int linkIdx = INVALID_IDX;
    sinkNodeIdx = INVALID_IDX;
    for (int i = 0; i < Nobjects[LINK]; i++)
    {
        if (Link[i].node1 == nodeIdx || Link[i].node2 == nodeIdx)
        {
            linkIdx = i;
            sinkNodeIdx = nodeIdx;
            break;
        }
    }

    return linkIdx;
}


int ICAP::findFirstNode(int sinkNodeType, int& subIndex)
{
    subIndex = INVALID_IDX;
    int nodeIdx = INVALID_IDX;

    for (int i = 0; i < Nobjects[NODE]; i++)
    {
        if (Node[i].type == sinkNodeType)
        {
            nodeIdx = i;
            subIndex = Node[i].subIndex;
            break;
        }
    }

    return nodeIdx;
}


bool ICAP::validateNetwork()
{
    if (Nnodes[STORAGE] > 1)
    {
        setError(1, "input file has more than one STORAGE element; only one may be present");
        return false;
    }
    else if (Nnodes[STORAGE] == 0)
    {
        setError(1, "input file has no STORAGE element; one must be present");
        return false;
    }

    return true;
}


int ICAP::findSources(SourceList& sourceList)
{
    for (int i = 0; i < Nobjects[LINK]; i++)
    {
        Source temp;
        temp.link = i;
        temp.active = false;

        // We only pick sources that are upstream of the current link.
        if (Node[ Link[i].node1 ].extInflow != NULL)
        {
            temp.node = Link[i].node1;
            temp.active = true;
        }
        //if (Node[ Link[i].node2 ].extInflow != NULL)
        //{
        //    temp.node = Link[i].node2;
        //    temp.active = true;
        //}

        if (temp.active)
            sourceList.AddSource(temp);
    }

	// Add reservoir as a source
	Source resSource;
	resSource.link = INVALID_IDX;
	resSource.active = true;
	int tempNode = INVALID_IDX;
	findSinkLink(STORAGE, tempNode);
	if (tempNode != INVALID_IDX && Node[tempNode].extInflow != NULL)
	{
		resSource.node = tempNode;
		sourceList.AddSource(resSource);
	}

    return sourceList.Count();
}


void ICAP::populateNetworkFromSWMM(ICAPNetwork& network)
{
    network.Clear();

    for (int i = 0; i < Nobjects[LINK]; i++)
    {
        ICAPNode* node = network.FindNode(Link[i].node1);
        if (node != NULL)
        {
            ICAPNode::link_iter iter = std::find(node->linkIdx.begin(), node->linkIdx.end(), i);
            if (iter == node->linkIdx.end())
                node->linkIdx.push_back(i);
        }
        else
            network.MakeNode(Link[i].node1, i);

        node = network.FindNode(Link[i].node2);
        if (node != NULL)
        {
            ICAPNode::link_iter iter = std::find(node->linkIdx.begin(), node->linkIdx.end(), i);
            if (iter == node->linkIdx.end())
                node->linkIdx.push_back(i);
        }
        else
            network.MakeNode(Link[i].node2, i);
    }
}
