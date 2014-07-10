#ifndef VARIABLES_H__
#define VARIABLES_H__


typedef double var_type;

namespace variables
{
    enum Variables
    {
        NodeDepth,
        NodeFlow,

        LinkFlow,
        LinkDsDepth,
        LinkUsDepth,
    };

    const var_type error_val = -99999;
}


#endif//VARIABLES_H__
