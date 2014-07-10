#ifndef STORAGE_H__
#define STORAGE_H__


#include "node.h"
#include "curveFactory.h"
#include "../api.h"


namespace geometry
{
    class ICAP_API StorageUnit : public Node
    {
        ICurveFactory* curveFactory;
        Curve* storageCurve;

    public:
        double funcCoeff;
        double funcExp;
        double funcConst;

        StorageUnit(const std::string& theId, ICurveFactory* factory, IModel* model);

        virtual bool parseLine(const std::vector<std::string>& parts);

        const Curve* getStorageCurve() { return this->storageCurve; }
    };
}


#endif//STORAGE_H__
