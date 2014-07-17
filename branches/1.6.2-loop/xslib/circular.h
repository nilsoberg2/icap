#ifndef CIRCULAR_H__
#define CIRCULAR_H__


#include "cross_section.h"

namespace xs
{
    class Circular : public CrossSection
    {
    private:
        double diameter;
    public:
        Circular();
        Circular(double diameter);
        void setDiameter(double diameter) { this->diameter = diameter; }
        virtual double computeAreaForDepth(double depth);
        virtual bool setParameters(std::vector<std::string>::const_iterator firstPart, std::vector<std::string>::const_iterator end);
    };
}


#endif//CIRCULAR_H__
