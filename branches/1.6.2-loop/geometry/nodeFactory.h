#ifndef NODEFACTORY_H__
#define NODEFACTORY_H__


#include <string>
#include "node.h"
#include "../api.h"


namespace geometry
{
    class Node;

    class ICAP_API INodeFactory
    {
    protected:
    public:
        virtual ~INodeFactory() {}
        virtual Node* getOrCreateNode(std::string nodeId) = 0;
    };
}


#endif//NODEFACTORY_H__
