#ifndef DUMMY_H__
#define DUMMY_H__


#include "cross_section.h"


namespace xs
{
    class Dummy : public CrossSection
    {
    public:
        Dummy() { CrossSection::xsType = xstype::dummy; }
        virtual double computeAreaForDepth(double depth) { return 0; }
        virtual bool setParameters(std::vector<std::string>::const_iterator firstPart, std::vector<std::string>::const_iterator end) { return true; }
    };
}


#endif//DUMMY_H__
