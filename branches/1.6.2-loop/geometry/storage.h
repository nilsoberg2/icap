#ifndef STORAGE_H__
#define STORAGE_H__

#include "../api.h"

#include "node.h"
#include "curve_factory.h"


namespace geometry
{
    class StorageUnit : public Node
    {
        std::shared_ptr<CurveFactory> curveFactory;
        std::shared_ptr<Curve> storageCurve;

    public:
        double funcCoeff;
        double funcExp;
        double funcConst;

        StorageUnit(const id_type& theId, const std::string& theName, std::shared_ptr<CurveFactory> factory);

        virtual bool parseLine(const std::vector<std::string>& parts);

        const std::shared_ptr<Curve> getStorageCurve() { return this->storageCurve; }

        virtual var_type lookupVolume(var_type depth) { return this->storageCurve->lookup(depth); }
    };
}


#endif//STORAGE_H__
