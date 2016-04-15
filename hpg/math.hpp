// ==============================================================================
// ICAP License
// ==============================================================================
// University of Illinois/NCSA
// Open Source License
// 
// Copyright (c) 2014-2016 University of Illinois at Urbana-Champaign.
// All rights reserved.
// 
// Developed by:
// 
//     Nils Oberg
// 	Blake J. Landry, PhD
// 	Arthur R. Schmidt, PhD
// 	Ven Te Chow Hydrosystems Lab
// 
//     University of Illinois at Urbana-Champaign
// 
//     https://vtchl.illinois.edu
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal with
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimers.
// 
//     * Redistributions in binary form must reproduce the above copyright notice,
//       this list of conditions and the following disclaimers in the
//       documentation and/or other materials provided with the distribution.
// 
//     * Neither the names of the Ven Te Chow Hydrosystems Lab, University of
// 	  Illinois at Urbana-Champaign, nor the names of its contributors may be
// 	  used to endorse or promote products derived from this Software without
// 	  specific prior written permission.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
// SOFTWARE.

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
