
#include <boost/algorithm/string.hpp>

#include "../util/parse.h"

#include "storage.h"


namespace geometry
{
    StorageUnit::StorageUnit(const id_type& theId, const std::string& theName, std::shared_ptr<CurveFactory> factory)
        : Node(theId, theName, NodeType::NodeType_Storage)
    {
        this->curveFactory = factory;
        this->storageCurve = NULL;
    }

    var_type StorageUnit::lookupVolume(var_type depth)
    {
        if (this->storageCurve == NULL)
        {
            return this->funcCoeff * std::pow(depth, this->funcExp) + this->funcConst;
        }
        else
        {
            return this->storageCurve->integrateUpTo(depth);
        }
    }

    bool StorageUnit::parseLine(const std::vector<std::string>& parts)
    {
        using namespace std;
        using namespace boost::algorithm;

        if (parts.size() < 6)
        {
            setErrorMessage("At least 6 parts are required for STORAGE line");
            return false;
        }

        if (!baseParseLine(parts))
        {
            return false;
        }

        if (to_upper_copy(parts[4]) == "FUNCTIONAL")
        {
            if (parts.size() < 8)
            {
                setErrorMessage("At least 8 parts are required for FUNCTIONAL curve");
                return false;
            }

            if (!tryParse(parts[5], this->funcCoeff))
            {
                setErrorMessage("Unable to parse A for FUNCTIONAL curve");
                return false;
            }

            if (!tryParse(parts[6], this->funcExp))
            {
                setErrorMessage("Unable to parse B for FUNCTIONAL curve");
                return false;
            }

            if (!tryParse(parts[7], this->funcConst))
            {
                setErrorMessage("Unable to parse C for FUNCTIONAL curve");
                return false;
            }
        }
        else
        {
            if (this->curveFactory == NULL)
            {
                setErrorMessage("No curve factory given");
                return false;
            }

            this->storageCurve = this->curveFactory->getOrCreateCurve(parts[5]);
        }
    }

}
