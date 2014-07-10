#ifndef CURVE_H__
#define CURVE_H__

#include <string>
#include <vector>
#include "parseable.h"
#include "../api.h"


namespace geometry
{
    class ICAP_API Curve : public Parseable
    {
    private:
        std::string name;
        std::string type;
        std::vector<double> xVals;
        std::vector<double> yVals;

        void Init(std::string theName, std::string theType);

    protected:
        Curve(std::string name, std::string type);

    public:
        Curve(std::string name);
        double lookup(double x) const;
        void addEntry(double x, double y);
        const std::string& getName() const;
        const std::string& getType() const;
        bool validate();
        bool parseLine(const std::vector<std::string>& parts);



        virtual void setErrorMessage(const std::string& msg) { Parseable::setErrorMessage("[" + this->name + "] " + msg); }
        virtual void appendErrorMessage(const std::string& msg) { Parseable::appendErrorMessage("[" + this->name + "] " + msg); }
    };
}

#endif//CURVE_H__
