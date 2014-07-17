#ifndef INFLOW_H__
#define INFLOW_H__

#include <vector>
#include <string>

#include "../util/parseable.h"
#include "../api.h"

#include "timeseries.h"
#include "timeseries_factory.h"


namespace geometry
{
    class Inflow : public Parseable
    {
    private:
        std::string inflowNodeName;
        std::string parameter;
        std::string paramType;

        std::shared_ptr<TimeseriesFactory> tsFactory;
        std::shared_ptr<Timeseries> timeseries;

        double scaleFactor;
        double baseLine;
        double unitsFactor;

    public:
        Inflow(std::shared_ptr<TimeseriesFactory> factory);

        bool parseLine(const std::vector<std::string>& parts);

        std::string getInflowNodeName() { return this->inflowNodeName; }

        virtual double getInflow(DateTime dateTime);

        void setInflowFactor(double flowFactor);
    };
}


#endif//INFLOW_H__
