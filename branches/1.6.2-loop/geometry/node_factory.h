#ifndef NODEFACTORY_H__
#define NODEFACTORY_H__


#include <string>
#include "node.h"
#include "../api.h"


namespace geometry
{
    class Node;

    class NodeFactory
    {
    protected:
    public:
        virtual ~NodeFactory() {}
        virtual std::shared_ptr<Node> getOrCreateNode(std::string nodeId) = 0;
    };
}


#endif//NODEFACTORY_H__
