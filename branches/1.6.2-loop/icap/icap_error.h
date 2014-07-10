#ifndef __ERROR_H___________________________20080422163232__
#define __ERROR_H___________________________20080422163232__
#include <string>
#include "common.h"

void error(char* msg);
void WriteErrorMsg(const char* msg);


#if defined(SWMM_GEOMETRY)

#define ERROR_SYSTEM            ERR_SYSTEM
#define ERROR_INPUT_FILE        ERR_INP_FILE
#define ERROR_INVALID_INPUT     ERR_INPUT
#define ERROR_ICAP_NETWORK      ERR_ICAP_NETWORK
#define ERROR_HPG_LOADING       ERR_HPG_LOADING

#else

#define ERROR_SYSTEM            100
#define ERROR_INPUT_FILE        110
#define ERROR_INVALID_INPUT     111
#define ERROR_ICAP_NETWORK      150
#define ERROR_HPG_LOADING       151

#endif

#endif//__ERROR_H___________________________20080422163232__
