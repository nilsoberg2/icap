

#include "../time/datetime.h"
#include "../model/units.h"
#include "../geometry/geometry.h"
#include "../util/parse.h"

#include "pumping.h"


Pumping::Pumping()
{
    this->pumpingTs = NULL;
    this->secondsSinceLastInflow = 0;
    this->pumpingRate = 0;
    this->daysBeforePumping = 10000;
    this->pumpingThreshold = 5;
}


bool Pumping::initializeSettings(geometry::Geometry* geometry)
{
    if (geometry->hasOption("pumping_ts"))
    {
        this->pumpingTs = geometry->getOrCreateTimeseries(geometry->getOption("pumping_ts"));
    }
    else
    {
        if (geometry->hasOption("pumping_rate"))
        {
            if (!tryParse(geometry->getOption("pumping_rate"), this->pumpingRate))
            {
                setErrorMessage("Unable to parse pumping_rate option");
                return false;
            }
        }
        else
        {
            return false;
        }
        
        if (geometry->hasOption("days_before_pumping"))
        {
            if (!tryParse(geometry->getOption("days_before_pumping"), this->daysBeforePumping))
            {
                setErrorMessage("Unable to parse days_before_pumping option");
                return false;
            }
        }
        else
        {
            return false;
        }

        if (geometry->hasOption("pumping_threshold"))
        {
            if (!tryParse(geometry->getOption("pumping_threshold"), this->pumpingThreshold))
            {
                setErrorMessage("Unable to parse pumping_threshold option");
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    
    // Start out the event-to-pumping interval counter with the correct duration to allow for
    // pumping from the start of a simulation.
    this->secondsSinceLastInflow = SECS_PER_DAY * this->daysBeforePumping;

    return true;
}


var_type Pumping::computePumpedVolume(var_type flowAtRes, double routeStep, DateTime currentDate)
{
    // If we have a pump timeseries, then return that volume.
    if (this->pumpingTs)
        return getPumpedVolume(routeStep, currentDate);
    
    // Otherwise do the static pumping rate computation.
    
	// If the flow at the reservoir is less than a given threshold, we
	// assume that no flow is entering and we start counting the seconds
	// since no flow has entered.  If Q at reservoir is >= the threshold
	// then we reset the counter to disable pumping.
    if (flowAtRes > this->pumpingThreshold)
        this->secondsSinceLastInflow = 0.0;
    else
    	this->secondsSinceLastInflow += routeStep;

	// If the last inflow counter is more than two days old, then we start
	// pumping out of the reservoir.
	if (this->secondsSinceLastInflow >= SECS_PER_DAY * this->daysBeforePumping) // # days (in seconds) since flow dropped below threshold
        return this->pumpingRate * UCF->flow() * routeStep;
    else
        return 0.0;
}


var_type Pumping::getPumpedRate(DateTime date)
{
    if (this->pumpingTs)
    {
        this->pumpingTs->lookup(date);
    }
    else
    {
        return 0.0;
    }
}


var_type Pumping::getPumpedVolume(double routeStep, DateTime date)
{
    var_type Q = getPumpedRate(date);

    return Q * UCF->flow() * routeStep;
}
