
#include "icap.h"
#include "icap_error.h"
#include <iostream>


void ICAP::setError(int code, std::string errorStr)
{
    m_errorStr = errorStr;
}


void ICAP::setError(int code, const char* errorStr)
{
    m_errorStr = errorStr;
}
/*

std::string ICAP::GetErrorStr()
{
    return m_errorStr;
}
*/

const char* ICAP::GetErrorStr()
{
    return m_errorStr.c_str();
}


void error(char* msg)
{
    std::cout << "\nERROR: " << msg << std::endl;
}


void WriteErrorMsg(const char* msg)
{
    if ( Frpt.file )
    {
        fprintf(Frpt.file, "\nICAP ERROR: %s\n", msg);
    }
}
