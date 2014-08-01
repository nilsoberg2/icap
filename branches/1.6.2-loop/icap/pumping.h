#ifndef PUMPING_H__
#define PUMPING_H__


#include <string>

#include "../type.h"
#include "../geometry/geometry.h"
#include "../util/parseable.h"


class Pumping : public Parseable
{
private:
    var_type pumpingThreshold;
    var_type daysBeforePumping;
    var_type pumpingRate;

    std::shared_ptr<geometry::Timeseries> pumpingTs;

    // Number of seconds since the last inflow to the system occured.
	var_type secondsSinceLastInflow;

    std::string errorMsg;

private:
    var_type getPumpedRate(DateTime date);
    var_type getPumpedVolume(double routeStep, DateTime date);

public:
    Pumping();

    bool initializeSettings(geometry::Geometry* geometry);

    var_type computePumpedVolume(var_type flowAtRes, double routeStep, DateTime currentDate);

    bool hasThreshold() { return pumpingThreshold > 0; }
    var_type getThreshold() { return pumpingThreshold; }
    bool hasPumpingTimeseries() { return pumpingTs != NULL; }
};


#endif//PUMPING_H__
