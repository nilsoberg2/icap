#ifndef __ROUTING_H_________________________20080424151717__
#define __ROUTING_H_________________________20080424151717__


enum RoutingRegime
{
    Regime_SteadyState = 1,     // Normal steady-state routing
    Regime_PondedDraining = 2,  // If the reservoir is empty, and we're pumping
    Regime_PondedFilling = 3,   // If the reservoir is empty, and we're filling
    Regime_Draining = 4,        // If the reservoir has water in it, and we're pumping
    Regime_Empty = 5,           // If everything is empty
};


#endif//__ROUTING_H_________________________20080424151717__
