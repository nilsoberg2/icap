#ifndef API_H__
#define API_H__


#ifdef ICAP_EXPORTS
#define ICAP_API _declspec(dllexport)
#else
#define ICAP_API _declspec(dllimport)
#endif


#define ERROR_VAL           -999999

#include "type.h"


#endif//API_H__
