#ifndef __COMMON_H__________________________20090902100303__
#define __COMMON_H__________________________20090902100303__


/////////////////////////////////////////////////////////////////////
// Include the SWMM headers.

#if defined(SWMM_GEOMETRY)

#define DLL
extern "C" {
    #include <headers.h>
    #include <swmm5.h>
}

#else

#define   MSECperDAY  8.64e7

#endif

#include "geomInterface.h"

/////////////////////////////////////////////////////////////////////

#define INVALID_IDX         -1
#define ERROR_VAL           -999999
#define ERROR_VAL_COMP      -999998 // this is the value to compare ERROR_VAL to (val < ERROR_VAL_COMP ==> val is ERROR VALUE)
#define IS_ERROR_VAL(val)   (val < ERROR_VAL_COMP)
#define ICAP_ZERO           0.00000001
#define IS_ZERO(val)        (fabs(##val##) < ICAP_ZERO)
#define IS_NOT_ZERO(val)    (fabs(##val##) >= ICAP_ZERO)

#define SECS_PER_2DAYS	172800.0
#define SECS_PER_DAY	86400.0


#endif//__COMMON_H__________________________20090902100303__
