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

#ifndef __SPLIT_HPP_________________________20080425101818__
#define __SPLIT_HPP_________________________20080425101818__


#include <vector>

//-----------------------------------------------------------
// StrT:    Type of string to be constructed
//          Must have char* ctor.
// str:     String to be parsed.
// delim:   Pointer to delimiter.
// results: Vector of StrT for strings between delimiter.
// empties: Include empty strings in the results. 
//-----------------------------------------------------------
// By Paul Wiess
// http://www.codeproject.com/KB/string/stringsplit.aspx

template< typename StrT >
int wiess_split(const char* str, const char* delim, 
                std::vector<StrT>& results, bool empties = true)
{
  char* pstr = const_cast<char*>(str);
  char* r = NULL;
  r = strstr(pstr, delim);
  int dlen = (int)strlen(delim);
  while( r != NULL )
  {
    char* cp = new char[(r-pstr)+1];
    memcpy(cp, pstr, (r-pstr));
    cp[(r-pstr)] = '\0';
    if( strlen(cp) > 0 || empties )
    {
      StrT s(cp);
      results.push_back(s);
    }
    delete[] cp;
    pstr = r + dlen;
    r = strstr(pstr, delim);
  }
  if( strlen(pstr) > 0 || empties )
  {
    results.push_back(StrT(pstr));
  }
  return (int)results.size();
}


#endif//__SPLIT_HPP_________________________20080425101818__
