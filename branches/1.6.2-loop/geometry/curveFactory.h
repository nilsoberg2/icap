#ifndef CURVEFACTORY_H__
#define CURVEFACTORY_H__


#include <string>
#include "curve.h"
#include "../api.h"


namespace geometry
{
    class ICAP_API ICurveFactory
    {
    protected:
    public:
        virtual ~ICurveFactory() {}
        virtual Curve* getOrCreateCurve(std::string curveName) = 0;
    };
}


#endif//CURVEFACTORY_H__
