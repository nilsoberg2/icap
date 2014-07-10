#ifndef MATH_H__
#define MATH_H__


#include <limits>


//-------------------------------------------------------------------
// FUNCTION:            isZero
// ARGUMENTS:           Number,HowManyEpsilons
// TEMPLATE ARGUMENTS:  blType
// PURPOSE:             Check if a number is zero by comparing it
//                      to the machine epsilon multiplied by the
//                      HowManyEpsilons argument
// DEPENDENCIES:        None
// AUTHOR:              https://www.barbato.us/2010/10/19/c-functions-working-numbers/#isZero
//-------------------------------------------------------------------
template<typename blType>
inline bool isZero(const blType& Number,
                   const blType HowManyEpsilons = 10)
{
    if(std::abs(Number) <= HowManyEpsilons * std::numeric_limits<blType>::epsilon())
        return true;
    else
        return false;
}
//-------------------------------------------------------------------


#endif//MATH_H__
