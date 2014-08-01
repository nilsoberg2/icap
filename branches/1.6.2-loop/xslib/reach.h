#ifndef REACH_H__
#define REACH_H__


#include <memory>

#include "cross_section.h"


namespace xs
{
    class Reach
    {
    private:
        std::shared_ptr<CrossSection> xs;
        double length;
        double dsInvert;
        double usInvert;
        double dsStation;
        double usStation;
        double roughness;

    public:
        Reach() : length(0), dsInvert(0), usInvert(0), dsStation(-99999), usStation(-99999), roughness(0) {}
        std::shared_ptr<CrossSection> getXs() const { return this->xs; }
        void setXs(std::shared_ptr<CrossSection> value) { this->xs = value; }

        const double getLength() const  { return this->length; }
		void setLength(double value) { this->length = value; }
        const double getDsInvert() const  { return this->dsInvert; }  
		void setDsInvert(double value) { this->dsInvert = value; }
        const double getUsInvert() const  { return this->usInvert; }  
		void setUsInvert(double value) { this->usInvert = value; }
        const double getDsStation() const  { return this->dsStation; }  
		void setDsStation(double value) { this->dsStation = value; }
        const double getUsStation() const  { return this->usStation; }  
		void setUsStation(double value) { this->usStation = value; }
        const double getRoughness() const  { return this->roughness; }  
        void setRoughness(double value) { this->roughness = value; }

        const double getMaxDepth() const  { return this->xs->getMaxDepth(); }  
        const double getSlope() const  { return (this->usInvert - this->dsInvert) / this->length; }  
    };
}


#endif//REACH_H__
