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

#ifndef __ICAP_INTERFACE_H__________________20080806160505__
#define __ICAP_INTERFACE_H__________________20080806160505__

#if defined(ICAP_EXPORT_DLL)
    #define ICAPDLLEXPORT __declspec(dllexport)
#else
    #if defined(ICAP_IMPORT_DLL)
        #define ICAPDLLEXPORT __declspec(dllimport)
    #else
        #define ICAPDLLEXPORT
    #endif
#endif


extern "C"
{

ICAPDLLEXPORT int __stdcall icap_open(int handle, char* inputFile, char* reportFile, char* outputFile);
ICAPDLLEXPORT int __stdcall icap_open_no_hpg(int handle, char* inputFile, char* reportFile, char* outputFile);
ICAPDLLEXPORT int __stdcall icap_start(int handle);
ICAPDLLEXPORT int __stdcall icap_step(int handle, double* curStep_in);
ICAPDLLEXPORT int __stdcall icap_end(int handle);
ICAPDLLEXPORT int __stdcall icap_close(int handle);

ICAPDLLEXPORT int __stdcall icap_count_hpg(int handle);
ICAPDLLEXPORT int __stdcall icap_load_hpg_step(int handle); // 0 no error, > 0 error, < 0 finished

ICAPDLLEXPORT int __stdcall icap_enable_rt_mode(int handle); // enable real-time mode, where results can be set and queried
ICAPDLLEXPORT int __stdcall icap_disable_rt_mode(int handle); // disable real-time mode, and just execute from input file

ICAPDLLEXPORT int __stdcall icap_clear_node_flows(int handle);
ICAPDLLEXPORT int __stdcall icap_add_source(int handle, char* nodeId);
ICAPDLLEXPORT int __stdcall icap_set_node_flow(int handle, char* nodeId, double flow);
ICAPDLLEXPORT int __stdcall icap_set_node_head(int h, char* nodeId, double head);
ICAPDLLEXPORT double __stdcall icap_get_node_head(int handle, char* nodeId);
ICAPDLLEXPORT double __stdcall icap_get_node_us_inflows(int handle, char* nodeId);

ICAPDLLEXPORT int __stdcall NewICAP();
int ICAPDLLEXPORT __stdcall DeleteICAP(int handle);

} // extern "C"

#endif//__ICAP_INTERFACE_H__________________20080806160505__
