
#include "icap.h"
#include <algorithm>


int ICAP::findSinkLink(int sinkNodeType, int& sinkNodeIdx)
{
    int nodeIdx = ICAP::findFirstNode(sinkNodeType);

    if (nodeIdx == INVALID_IDX)
        return INVALID_IDX;

    int linkIdx = INVALID_IDX;
    sinkNodeIdx = INVALID_IDX;
    for (int i = 0; i < GLINK_COUNT; i++)
    {
        if (GLINK_USNODE(i) == nodeIdx || GLINK_DSNODE(i) == nodeIdx)
        {
            linkIdx = i;
            sinkNodeIdx = nodeIdx;
            break;
        }
    }

    return linkIdx;
}


int ICAP::findFirstNode(int sinkNodeType)
{
    int nodeIdx = INVALID_IDX;

    for (int i = 0; i < GNODE_COUNT; i++)
    {
        if (GNODE_TYPE(i) == sinkNodeType)
        {
            nodeIdx = i;
            break;
        }
    }

    return nodeIdx;
}


bool ICAP::validateNetwork()
{
    if (GNODE_TYPECOUNT(STORAGE) > 1)
    {
        setError(1, "input file has more than one STORAGE element; only one may be present");
        return false;
    }
    else if (GNODE_TYPECOUNT(STORAGE) == 0)
    {
        setError(1, "input file has no STORAGE element; one must be present");
        return false;
    }

    return true;
}


int ICAP::findSources(SourceList& sourceList)
{
    for (int i = 0; i < GLINK_COUNT; i++)
    {
        Source temp;
        temp.link = i;
        temp.active = false;

        // We only pick sources that are upstream of the current link.
        if (GNODE_HASINFLOW(GLINK_USNODE(i)))
        {
            temp.node = GLINK_USNODE(i);
            temp.active = true;
        }
        //if (Node[ GLINK_DSNODE(i) ].extInflow != NULL)
        //{
        //    temp.node = GLINK_DSNODE(i);
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
	if (tempNode != INVALID_IDX && GNODE_HASINFLOW(tempNode))
	{
		resSource.node = tempNode;
		sourceList.AddSource(resSource);
	}

    return sourceList.Count();
}


void ICAP::populateNetworkFromSWMM(ICAPNetwork& network)
{
    network.Clear();

    for (int i = 0; i < GLINK_COUNT; i++)
    {
        ICAPNode* node = network.FindNode(GLINK_USNODE(i));
        if (node != NULL)
        {
            ICAPNode::link_iter iter = std::find(node->linkIdx.begin(), node->linkIdx.end(), i);
            if (iter == node->linkIdx.end())
                node->linkIdx.push_back(i);
        }
        else
            network.MakeNode(GLINK_USNODE(i), i);

        node = network.FindNode(GLINK_DSNODE(i));
        if (node != NULL)
        {
            ICAPNode::link_iter iter = std::find(node->linkIdx.begin(), node->linkIdx.end(), i);
            if (iter == node->linkIdx.end())
                node->linkIdx.push_back(i);
        }
        else
            network.MakeNode(GLINK_DSNODE(i), i);
    }
}
