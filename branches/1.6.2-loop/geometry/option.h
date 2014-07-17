#ifndef OPTION_H__
#define OPTION_H__


#include <string>

#include "../util/parseable.h"


namespace geometry
{
    class Option : public Parseable
    {
    private:
        std::string name;
        std::string value;

    public:
        
        std::string getName() { return this->name; }
        std::string getValue() { return this->value; }

        virtual bool parseLine(const std::string& line);

        std::string toString() { return name + "=" + value; }
    };
}


#endif//OPTION_H__
