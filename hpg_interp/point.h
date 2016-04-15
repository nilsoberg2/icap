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
//     Blake J. Landry, PhD
//     Arthur R. Schmidt, PhD
//     Ven Te Chow Hydrosystems Lab
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

#ifndef POINT_H__
#define POINT_H__



// Begin wrapping code in the HPG namespace
namespace hpg
{


    // This structure defines an elevation tuple.  One tuple holds an X value,
    // a Y value, and a Volume value.

    struct point
    {
        double  x;
        bool    x_valid;
        double  y;
        bool    y_valid;
        double  v;
        bool    v_valid;
		double  hf;
		bool    hf_valid;
        point(const double& x_, const double& y_, const double& v_, const double& hf_)
			{x=x_; y=y_; v=v_; hf=hf_; x_valid = y_valid = v_valid = hf_valid = true;}
        point(const double& x_, const double& y_, const double& v_)
			{x=x_; y=y_; v=v_; x_valid = y_valid = v_valid = true; hf_valid = false;}
        point(const double& x_, const double& y_)
			{x=x_; y=y_; x_valid = y_valid = true; v_valid = hf_valid = false;}
        point()
			{x=0.0; y=0.0; v=0.0; hf = 0.0; x_valid = y_valid = v_valid = hf_valid = false;}
        point& operator=(const point& in)
			{x=in.x; y=in.y; v=in.v; hf=in.hf; x_valid=in.x_valid; y_valid=in.y_valid; v_valid=in.v_valid; hf_valid=in.hf_valid; return *this;}
    };

}


#endif//POINT_H__
