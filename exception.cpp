#define _CRT_SECURE_NO_DEPRECATE


#include "exception.h"
#include "icap.h"

#if defined(USE_EXCEPTION_HANDLER)

#ifdef _MSC_VER
#include <windows.h>
#include <excpt.h>
#endif

static int  ICAPExceptionCount;
#define MAX_ICAP_EXCEPTIONS 100


int ICAPExceptionHandler(int xc, double time, int stepCount, const char* errorMsg)
//
//  Input:   xc          = exception code
//           elapsedTime = simulation time when exception occurred (days)
//           step        = step count at time when exception occurred
//  Output:  returns an exception handling code
//  Purpose: exception filtering routine for operating system exceptions
//           under Windows.
//
{
    int  rc;                           // result code
    long hour;                         // current hour of simulation
    char msg[40];                      // exception type text
    char xmsg[1024];                   // error message text
    switch (xc)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        sprintf(msg, "\n  Access violation ");
        rc = EXCEPTION_EXECUTE_HANDLER;
        break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        sprintf(msg, "\n  Illegal floating point operand ");
        rc = EXCEPTION_CONTINUE_EXECUTION;
        break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        sprintf(msg, "\n  Floating point divide by zero ");
        rc = EXCEPTION_CONTINUE_EXECUTION;
        break;
    case EXCEPTION_FLT_INVALID_OPERATION:
        sprintf(msg, "\n  Illegal floating point operation ");
        rc = EXCEPTION_CONTINUE_EXECUTION;
        break;
    case EXCEPTION_FLT_OVERFLOW:
        sprintf(msg, "\n  Floating point overflow ");
        rc = EXCEPTION_CONTINUE_EXECUTION;
        break;
    case EXCEPTION_FLT_STACK_CHECK:
        sprintf(msg, "\n  Floating point stack violation ");
        rc = EXCEPTION_EXECUTE_HANDLER;
        break;
    case EXCEPTION_FLT_UNDERFLOW:
        sprintf(msg, "\n  Floating point underflow ");
        rc = EXCEPTION_CONTINUE_EXECUTION;
        break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        sprintf(msg, "\n  Integer divide by zero ");
        rc = EXCEPTION_CONTINUE_EXECUTION;
        break;
    case EXCEPTION_INT_OVERFLOW:
        sprintf(msg, "\n  Integer overflow ");
        rc = EXCEPTION_CONTINUE_EXECUTION;
        break;
    default:
        sprintf(msg, "\n  Exception %d", xc);
        rc = EXCEPTION_EXECUTE_HANDLER;
    }
    hour = (long)(time / 1000.0 / 3600.0);
    sprintf(xmsg, "%s at step %d, hour %d\nerror message: %s\n", msg, stepCount, hour, errorMsg);
    if ( rc == EXCEPTION_EXECUTE_HANDLER ||
         ++ICAPExceptionCount >= MAX_ICAP_EXCEPTIONS )
    {
        strcat(xmsg, " --- execution halted.");
        rc = EXCEPTION_EXECUTE_HANDLER;
    }
    report_writeLine(xmsg);
    return rc;
}


#endif
