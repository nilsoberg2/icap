#ifndef UCF_H__
#define UCF_H__


#include "../type.h"
#include "../model/units.h"


class CfsUnitsConversion : public ucf::Ucf
{
public:
    CfsUnitsConversion();
    virtual var_type flow();
};


#endif//UCF_H__
