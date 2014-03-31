#ifndef __ROUTING_H_________________________20080424151717__
#define __ROUTING_H_________________________20080424151717__


#define LINK_DS_INVERT(linkIdx)     (Node[ Link[##linkIdx##].node2 ].invertElev+Link[##linkIdx##].offset2)
#define LINK_US_INVERT(linkIdx)     (Node[ Link[##linkIdx##].node1 ].invertElev+Link[##linkIdx##].offset1)
#define LINK_DIAMETER(linkIdx)      (Link[##linkIdx##].xsect.yFull)
#define LINK_LENGTH(linkIdx)        (Conduit[Link[##linkIdx##].subIndex].length)
#define LINK_ROUGHNESS(linkIdx)     (Conduit[Link[##linkIdx##].subIndex].roughness)
#define LINK_SLOPE(linkIdx)         (Conduit[Link[##linkIdx##].subIndex].slope)
#define LINK_HASGEOM(linkIdx)       (Link[##linkIdx##].xsect.type != DUMMY)

// Returns elevation in ponded case
#define LINK_US_DEPTH(linkIdx)      (Link[##linkIdx##].newDepth)
// Returns elevation in ponded case
#define LINK_DS_DEPTH(linkIdx)      (Link[##linkIdx##].oldDepth)

#define PRESSURIZED_THRESHOLD           0.8
#define IS_PRESSURIZED_IDX(idx, depth)  ((depth) > LINK_DIAMETER(idx)*PRESSURIZED_THRESHOLD)
#define IS_PRESSURIZED(depth, diam)     ((depth) > (diam)*PRESSURIZED_THRESHOLD)

#define GATE_LOSS_DISABLED


enum RoutingRegime
{
    Regime_SteadyState = 1,     // Normal steady-state routing
    Regime_PondedDraining = 2,  // If the reservoir is empty, and we're pumping
    Regime_PondedFilling = 3,   // If the reservoir is empty, and we're filling
    Regime_Draining = 4,        // If the reservoir has water in it, and we're pumping
    Regime_Empty = 5,           // If everything is empty
};


#endif//__ROUTING_H_________________________20080424151717__
