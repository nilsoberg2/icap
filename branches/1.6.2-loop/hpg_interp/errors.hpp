#ifndef __ERRORS_HPP________________________20051114145959__
#define __ERRORS_HPP________________________20051114145959__

#include "../hpg/error.hpp"


namespace hpg
{
    namespace err
    {
        const int InvalidFlow = 1;
        const int InvalidInput = 2;
        const int InvalidParam = 3;
        const int GenericInterpFailed = 4;
        const int InvalidSplineSize = 5;
        const int GenericSplineError = 6;
        const int FileReadFailed = 7;
        const int FileWriteFailed = 8;
        const int InvalidFileFormat = 9;
        const int CantComputeNormCrit = 10;
    }
}


#define CLEARERR          errorCode=0;


#endif//__ERRORS_HPP________________________20051114145959__
