#ifndef CURVE_H__
#define CURVE_H__

#include <string>
#include <vector>

#include "../util/parseable.h"
#include "../api.h"


namespace geometry
{
    class Curve : public Parseable
    {
    private:
        std::string name;
        std::string type;
        std::vector<var_type> xVals;
        std::vector<var_type> yVals;

        void Init(std::string theName, std::string theType);

    protected:
        Curve(std::string name, std::string type);

    public:
        Curve(std::string name);
        
        var_type lookup(var_type x) const;
        var_type integrateUpTo(var_type x) const;
        var_type inverseLookup(var_type y) const;

        void addEntry(var_type x, var_type y);
        const std::string& getName() const;
        void setName(std::string& theName) { this->name = theName; }
        const std::string& getType() const;
        bool validate();
        bool parseLine(const std::vector<std::string>& parts);
        
        void getFirstPoint(var_type& x, var_type& y);
        void getLastPoint(var_type& x, var_type& y);

        virtual void setErrorMessage(const std::string& msg) { Parseable::setErrorMessage("[" + this->name + "] " + msg); }
        virtual void appendErrorMessage(const std::string& msg) { Parseable::appendErrorMessage("[" + this->name + "] " + msg); }
    };
}

#endif//CURVE_H__
