#ifndef __MATH_HPP__________________________20050103154545__
#define __MATH_HPP__________________________20050103154545__
#define _USE_MATH_DEFINES
#include <cmath>
#undef _USE_MATH_DEFINES


#if !defined(M_PI)
#define M_PI		3.14159265358979323846
#endif
#if !defined(M_SQRT2)
#define M_SQRT2		1.41421356237309504880
#endif


//#if !defined(min)
////		template <typename p_type> p_type min(p_type X, p_type Y) { return (X < Y ? X : Y); }
//#define min(X, Y)	(X < Y ? X : Y)
//#endif
//#if !defined(max)
////		template <typename p_type> p_type max(p_type X, p_type Y) { return (X > Y ? X : Y); }
//#define max(X, Y)	(X > Y ? X : Y)
//#endif
#if !defined(round)
		template <typename p_type> p_type round(p_type X) { return (p_type)floor(X + 0.5); }
		template <typename p_type> p_type round(p_type X, p_type Y) { return (p_type)( floor((double)X/(double)Y)*Y + floor((double)((long)X%(long)Y)/Y+0.5)*Y ); }
		template <typename p_type, typename r_type> r_type round(p_type X) { return (r_type)floor(X + 0.5); }
#endif



#endif//__MATH_HPP__________________________20050103154545__
