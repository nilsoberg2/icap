#ifndef DUMMY_H__
#define DUMMY_H__


#include "cross_section.h"


namespace xs
{
    class Dummy : public CrossSection
    {
    public:
        Dummy() { CrossSection::xsType = xstype::dummy; }
        virtual bool setParameters(std::vector<std::string>::const_iterator firstPart, std::vector<std::string>::const_iterator end) { return true; }

        virtual double getMaxDepth() { return 0; }

        virtual double computeArea(double depth) { return 0; }
        virtual double computeWettedPerimiter(double depth) { return 0; }
        virtual double computeTopWidth(double depth) { return 0; }
        virtual double computeDpDy(double depth) { return 0; }
        virtual double computeDaDy(double depth) { return 0; }
        virtual double computeDtDy(double depth) { return 0; }
    };
}


#endif//DUMMY_H__
