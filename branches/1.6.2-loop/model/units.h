#ifndef UNITS_H__
#define UNITS_H__


#include "../api.h"
#include "../type.h"


namespace units
{
    enum UnitSystem
    {
        Units_SI = 0,
        Units_US = 1,
    };

    class Units
    {
    private:
        units::UnitSystem system;
    public:
        Units(units::UnitSystem system);

        var_type g();

    };
}


extern units::Units* UCS;


namespace ucf
{
    enum UnitConversionFactors
    {
        CFS
    };

    class Ucf
    {
    private:
    public:
        virtual var_type flow() = 0;
    };
}

extern ucf::Ucf* UCF;


#endif//UNITS_H__
