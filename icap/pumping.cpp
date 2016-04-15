// ==============================================================================
// ICAP License
// ==============================================================================
// University of Illinois/NCSA
// Open Source License
// 
// Copyright (c) 2014-2016 University of Illinois at Urbana-Champaign.
// All rights reserved.
// 
// Developed by:
// 
//     Nils Oberg
//     Blake J. Landry, PhD
//     Arthur R. Schmidt, PhD
//     Ven Te Chow Hydrosystems Lab
// 
//     University of Illinois at Urbana-Champaign
// 
//     https://vtchl.illinois.edu
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal with
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimers.
// 
//     * Redistributions in binary form must reproduce the above copyright notice,
//       this list of conditions and the following disclaimers in the
//       documentation and/or other materials provided with the distribution.
// 
//     * Neither the names of the Ven Te Chow Hydrosystems Lab, University of
// 	  Illinois at Urbana-Champaign, nor the names of its contributors may be
// 	  used to endorse or promote products derived from this Software without
// 	  specific prior written permission.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
// SOFTWARE.



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


bool Pumping::initializeSettings(std::shared_ptr<geometry::Geometry> geometry)
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
