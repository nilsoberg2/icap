#ifndef __ICAPNETWORK_H_____________________20080424145858__
#define __ICAPNETWORK_H_____________________20080424145858__

#include <vector>
#include <map>

struct ICAPPair
{
    double x, y;
    ICAPPair(double x_, double y_);
};

struct ICAPLink
{
    int swmmIdx;
    double x1, y1, x2, y2;
    std::vector<ICAPPair> verts;
    int vertCount;

    ICAPLink();
    ~ICAPLink();
};


// This represents the SWMM index of a given node and all of the
// links connecting to that node.
struct ICAPNode
{
    int swmmIdx;
    std::vector<int> linkIdx;
    typedef std::vector<int>::iterator link_iter;
};


// This is a representation of a network based on the SWMM network.
// The problem with the SWMM network representation is that nodes
// have no knowledge of connecting links.  So we need to create a
// network that allows us to access any part of the network from an
// arbitrary link or node.  This is meant to be used in cooperation
// with SWMM's network (using the Node and Link arrays).
class ICAPNetwork
{
protected:
    std::map<int, ICAPNode*> nodeList;
    typedef std::map<int, ICAPNode*>::iterator node_iter;

    std::map<int, ICAPLink*> linkList;
    typedef std::map<int, ICAPLink*>::iterator link_iter;

public:
    ~ICAPNetwork();
    void Clear();

    // Find a node in the network.
    ICAPNode* FindNode(int node_idx);

    // Find a link in the network.
    ICAPLink* FindLink(int link_idx);

    // Create a new node in the network.
    void MakeNode(int node_idx, int linkIdx);

    // Create a new link in the network.
    ICAPLink* MakeLink(int linkIdx);
};


#endif//__ICAPNETWORK_H_____________________20080424145858__
