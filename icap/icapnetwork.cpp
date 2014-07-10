
#include "icapnetwork.h"


ICAPPair::ICAPPair(double x_, double y_)
{
    x = x_;
    y = y_;
}


ICAPLink::ICAPLink()
{
    vertCount = 0;
    x1 = y1 = x2 = y2 = 0.0;
    swmmIdx = -1;
}

ICAPLink::~ICAPLink()
{
}


void ICAPNetwork::Clear()
{
    node_iter it1 = nodeList.begin();
    while (it1 != nodeList.end())
    {
        if (it1->second != NULL)
            delete it1->second;
        it1++;
    }
    nodeList.clear();

    link_iter it2 = linkList.begin();
    while (it2 != linkList.end())
    {
        if (it2->second != NULL)
            delete it2->second;
        it2++;
    }
    linkList.clear();
}


ICAPNetwork::~ICAPNetwork()
{
    Clear();
}


ICAPNode* ICAPNetwork::FindNode(int node_idx)
{
    node_iter it = nodeList.find(node_idx);
    if (it == nodeList.end())
        return NULL;
    else
        return it->second;
}


ICAPLink* ICAPNetwork::FindLink(int linkIdx)
{
    link_iter it = linkList.find(linkIdx);
    
    if (it == linkList.end())
        return NULL;
    else
        return it->second;
}


void ICAPNetwork::MakeNode(int node_idx, int linkIdx)
{
    if (FindNode(node_idx) != NULL)
        return;

    ICAPNode* node = new ICAPNode();
    node->swmmIdx = node_idx;
    node->linkIdx.push_back(linkIdx);

    nodeList.insert(std::make_pair(node_idx, node));
}


ICAPLink* ICAPNetwork::MakeLink(int linkIdx)
{
    ICAPLink* link;
    if ((link = FindLink(linkIdx)) != NULL)
        return link;

    link = new ICAPLink();
    link->swmmIdx = linkIdx;

    linkList.insert(std::make_pair(linkIdx, link));

    return link;
}
