#ifndef MODELELEMENT_H__
#define MODELELEMENT_H__


#include "model.h"
#include "variables.h"


class IModelElement
{
public:
    virtual ~IModelElement() {}
    virtual void resetFlow() = 0;
    virtual void resetDepth() = 0;
    virtual var_type& variable(variables::Variables var) = 0;

    //virtual void propagateDepthUpstream(double depth) = 0;
};


#endif//MODELELEMENT_H__
