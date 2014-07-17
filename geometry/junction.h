#ifndef JUNCTION_H__
#define JUNCTION_H__


#include "node.h"
#include "../api.h"


namespace geometry
{
    class Junction : public Node
    {
    public:
        Junction(const id_type& theId, const std::string& theName)
            : Node(theId, theName, NodeType::NodeType_Junction)
        {
        }

        virtual bool parseLine(const std::vector<std::string>& parts)
        {
            return baseParseLine(parts);
        }
    };
}


#endif//JUNCTION_H__
