#ifndef TIMESERIESFACTORY_H__
#define TIMESERIESFACTORY_H__


#include <string>
#include "timeseries.h"
#include "../api.h"


namespace geometry
{
    class Timeseries;

    class TimeseriesFactory
    {
    protected:
    public:
        virtual ~TimeseriesFactory() {}
        virtual std::shared_ptr<Timeseries> getOrCreateTimeseries(std::string tsId) = 0;
    };
}


#endif//TIMESERIESFACTORY_H__
