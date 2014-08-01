#ifndef API_H__
#define API_H__


#ifdef _USRDLL
    #ifdef ICAP_EXPORTS
    #define ICAP_API _declspec(dllexport)
    #else
    #define ICAP_API _declspec(dllimport)
    #endif
#else
#define ICAP_API
#endif


#define ERROR_VAL           -999999

#include "type.h"


#endif//API_H__
