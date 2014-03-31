#ifndef __EXCEPTION_H_______________________20080821164747__
#define __EXCEPTION_H_______________________20080821164747__

//#define USE_EXCEPTION_HANDLER

#if defined(_MSC_VER) && defined(USE_EXCEPTION_HANDLER)

#define GetExceptionCode            _exception_code // from excpt.h
extern "C" unsigned long __cdecl    _exception_code(void); // from excpt.h

#define EXCEPTION_HANDLER(T, C, S)  ICAPExceptionHandler(GetExceptionCode(), T, C, S)
#define EXCEPTION_TRY               __try {
#define EXCEPTION_CATCH(T,C,S,A)    } __except(EXCEPTION_HANDLER(T,C,S)) { A; }
#define EXCEPTION_END               }

int ICAPExceptionHandler(int exCode, double time, int stepCount, const char* errorMsg);

#else
#define EXCEPTION_HANDLER(T, C, S)
#define EXCEPTION_TRY               
#define EXCEPTION_CATCH(T,C,S,A)    
#define EXCEPTION_END               
#endif


#endif//__EXCEPTION_H_______________________20080821164747__
