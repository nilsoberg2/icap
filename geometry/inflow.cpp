
#include <boost/algorithm/string.hpp>
#include <string>

#include "../util/parse.h"

#include "inflow.h"


namespace geometry
{
    Inflow::Inflow(std::shared_ptr<TimeseriesFactory> factory)
    {
        this->tsFactory = factory;
        this->timeseries = NULL;
        this->scaleFactor = 1;
        this->unitsFactor = 1; // Units factor
        this->baseLine = 0;
    }

    void Inflow::setInflowFactor(double flowFactor)
    {
        this->scaleFactor = flowFactor;
    }

    bool Inflow::parseLine(const std::vector<std::string>& parts)
    {
        using namespace std;
        using namespace boost::algorithm;

        if (parts.size() < 3)
        {
            setErrorMessage("At least 3 parts required for INFLOW line");
            return false;
        }

        this->inflowNodeName = parts[0];

        if (parts[1] != "flow")
        {
            setErrorMessage("Invalid field 2 in INFLOW line: expecting FLOW");
            return false;
        }

        string tsName = trim_copy(parts[2]);
        if (tsName.size() > 0)
        {
            this->timeseries = this->tsFactory->getOrCreateTimeseries(tsName);
        }
        else
        {
            this->timeseries = NULL;
        }

        if (parts.size() > 5)
        {
            if (!tryParse(parts[5], this->scaleFactor))
            {
                setErrorMessage("Unable to parse TS scale factor");
                return false;
            }
        }

        if (parts.size() > 6)
        {
            if (!tryParse(parts[6], this->baseLine))
            {
                setErrorMessage("Unable to parse TS base line");
                return false;
            }
        }

        return true;
    }

    
    double Inflow::getInflow(DateTime dateTime)
    {
        double tsVal = 0;
        if (this->timeseries != NULL)
        {
            tsVal = this->timeseries->lookup((double)dateTime) * this->scaleFactor;
        }

        tsVal += this->baseLine;
        tsVal *= this->unitsFactor;

        return tsVal;
    }
}
