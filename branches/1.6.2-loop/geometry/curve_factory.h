#ifndef CURVEFACTORY_H__
#define CURVEFACTORY_H__


#include <string>
#include "curve.h"
#include "../api.h"


namespace geometry
{
    class CurveFactory
    {
    protected:
    public:
        virtual ~CurveFactory() {}
        virtual std::shared_ptr<Curve> getOrCreateCurve(std::string curveName) = 0;
    };
}


#endif//CURVEFACTORY_H__
