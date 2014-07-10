#ifndef JUNCTION_H__
#define JUNCTION_H__


#include "node.h"
#include "../api.h"


namespace geometry
{
    class ICAP_API Junction : public Node
    {
    public:
        Junction(const std::string& theId, IModel* model)
            : Node(theId, model)
        {
            this->nodeType = NodeType::NodeType_Junction;
        }

        virtual bool parseLine(const std::vector<std::string>& parts)
        {
            return baseParseLine(parts);
        }
    };
}


#endif//JUNCTION_H__
