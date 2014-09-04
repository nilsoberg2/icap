#ifndef CROSS_SECTION_H__
#define CROSS_SECTION_H__

#include <string>
#include <vector>
#include <memory>

#include "../api.h"
#include "../util/parseable.h"

#include "types.h"


namespace xs
{
    class CrossSection : public Parseable
    {
    protected:
        xstype xsType;
    public:
        virtual ~CrossSection() { }
        virtual bool setParameters(std::vector<std::string>::const_iterator firstPart, std::vector<std::string>::const_iterator end) = 0;

        virtual xstype getType() const { return this->xsType; }
        virtual double getMaxDepth() = 0;

        virtual double computeArea(double depth) = 0;
        virtual double computeWettedPerimiter(double depth) = 0;
        virtual double computeTopWidth(double depth) = 0;
        virtual double computeDpDy(double depth) = 0;
        virtual double computeDaDy(double depth) = 0;
        virtual double computeDtDy(double depth) = 0;

        virtual std::shared_ptr<CrossSection> clone() = 0;
    };

    class Factory
    {
    public:
        static CrossSection* create(const CrossSection* xs);
        static CrossSection* create(xstype xsType);
        static CrossSection* create(const std::string& type);
        template<typename XSType>
        static CrossSection* create();
    };
}


#endif//CROSS_SECTION_H__
