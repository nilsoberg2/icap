#ifndef UNITS_H__
#define UNITS_H__


#include "../api.h"

enum UnitSystem
{
    Units_SI = 0,
    Units_US = 1,
};


class ICAP_API Units
{
private:
    UnitSystem system;
public:
    Units(UnitSystem system);

    double g();

};


extern ICAP_API Units* UCS;


#endif//UNITS_H__
