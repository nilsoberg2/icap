#ifndef NODELIST_H__
#define NODELIST_H__

#include <memory>

#include "node.h"


namespace geometry
{
    class NodeList
    {
    public:
        virtual int count() = 0;
        virtual std::shared_ptr<Node> get(int index) = 0;
        virtual id_type id(int index) = 0;
    };

    class NodeList_helper : public NodeList
    {
    public:
        virtual int node_count() = 0;
        virtual int count() { return node_count(); }

        virtual std::shared_ptr<Node> node_get(int index) = 0;
        virtual std::shared_ptr<Node> get(int index) { return node_get(index); }

        virtual id_type node_id(int index) = 0;
        virtual id_type id(int index) { return node_id(index); }
    };
}


#endif//NODELIST_H__
