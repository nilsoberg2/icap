#ifndef TYPES_HPG_H__
#define TYPES_HPG_H__

#include <deque>

#include "../api.h"

#include "point.h"


namespace hpg
{
    struct intpair
    {
        int x, y;
    };

    typedef std::deque<point> hpgvec;

    enum InterpValue
    {
        Interp_Downstream,
        Interp_Upstream,
        Interp_Volume,
		Interp_Hf,
    };
}


#endif//TYPES_HPG_H__
