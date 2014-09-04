#ifndef VARIABLES_H__
#define VARIABLES_H__

#include "../type.h"


namespace variables
{
    enum Variables
    {
        NodeDepth,
        NodeFlow,
        //NodeInflow,
        NodeLateralInflow, // All inflow coming into the node from an external hydrograph
        NodeOverflow,
        NodeVolume,

        LinkFlow,
        LinkDsDepth,
        LinkUsDepth,
        LinkVolume,
    };

    const var_type error_val = -99999;
}


namespace statvariables
{
    enum StatVariables
    {
        FloodedNodes,
        PumpedVolume,
    };
}


#endif//VARIABLES_H__
