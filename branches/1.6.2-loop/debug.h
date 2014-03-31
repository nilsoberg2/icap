#ifndef __DEBUG_H___________________________20080424150101__
#define __DEBUG_H___________________________20080424150101__


#if defined(_DEBUG) || defined(DEBUG) || defined(ICAP_DEBUG)

#include <cstdio>


#define ICAP_DEBUGGING
#define DEBUG_OPEN
#define dprintf(...) fprintf(m_debugFile,  __VA_ARGS__);
#define DEBUG_CLOSE

#define DEBUG_GETCHAR   getchar();

#define DEBUG_EXIT      { DEBUG_GETCHAR; exit(1); }

void OutputTimeStep(char* datetime, FILE* fh);


#else

#define DEBUG_OPEN
#define dprintf(...)
#define DEBUG_CLOSE

#define DEBUG_GETCHAR

#define DEBUG_EXIT      exit(1);

#endif



#endif//__DEBUG_H___________________________20080424150101__
