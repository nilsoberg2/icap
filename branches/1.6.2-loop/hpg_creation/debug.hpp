#ifndef __DEBUG_HPP_________________________20080505103535__
#define __DEBUG_HPP_________________________20080505103535__


#if defined(WIN32) && defined(_DEBUG)

#include <iostream>

#define DEBUGMSG(x)  std::cout << x << std::endl;

#else

#define DEBUGMSG(x)

#endif



#endif//__DEBUG_HPP_________________________20080505103535__
