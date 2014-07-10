#ifndef __UTIL_H____________________________20080428143535__
#define __UTIL_H____________________________20080428143535__

#include "icapnetwork.h"


extern "C"
{
int FileExists(const char* strFilename);
}


int TSDateTimeStrLen();
void TSDateTimeStr(double currentDate, char* datetimeBuf);

bool GetSWMMCoordsAndVerts(const char* file, ICAPNetwork& network);


#endif//__UTIL_H____________________________20080428143535__
