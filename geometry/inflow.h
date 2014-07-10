#ifndef INFLOW_H__
#define INFLOW_H__

#include <vector>
#include <string>
#include "timeseries.h"
#include "timeseriesFactory.h"
#include "parseable.h"
#include "../api.h"


namespace geometry
{
    class ICAP_API Inflow : public Parseable
    {
    private:
        std::string inflowNodeName;
        std::string parameter;
        std::string paramType;

        ITimeseriesFactory* tsFactory;
        Timeseries* timeseries;

        double scaleFactor;
        double baseLine;
        double unitsFactor;

    public:
        Inflow(ITimeseriesFactory* factory);

        bool parseLine(const std::vector<std::string>& parts);

        std::string getInflowNodeName() { return this->inflowNodeName; }

        virtual double getInflow(DateTime dateTime);

        void setInflowFactor(double flowFactor);
    };
}


#endif//INFLOW_H__
