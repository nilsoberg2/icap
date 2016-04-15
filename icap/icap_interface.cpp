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


// INTERFACE SO THAT GUI CAN COMMUNICATE WITH ICAP

#include "icap.h"
#include "icap_interface.h"

extern "C"
{

    typedef ICAP* ICAPHandle;

int __stdcall NewICAP()
{
    return (int)(new ICAP());
}


int __stdcall DeleteICAP(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle != NULL)
        delete handle;
    return 0;
}


int __stdcall icap_count_hpg(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    return handle->GetLinkCount();
}


int __stdcall icap_load_hpg_step(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    return handle->loadNextHpg();
}


int __stdcall icap_open(int h, char* inputFile, char* reportFile, char* outputFile)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;
    
    bool result = handle->Open(inputFile, reportFile, outputFile, true);  // true to load HPGs
    return (result ? 0 : 1);
}


int __stdcall icap_open_no_hpg(int h, char* inputFile, char* reportFile, char* outputFile)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;
    
    bool result = handle->Open(inputFile, reportFile, outputFile, false);  // false to not load HPGs
    return (result ? 0 : 1);
}


int __stdcall icap_start(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;
    
    bool result = handle->Start();
    return (result ? 0 : 1);
}


int __stdcall icap_end(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;
    
    bool result = handle->End();
    return (result ? 0 : 1);
}


int __stdcall icap_close(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;
    
    bool result = handle->Close();
    return (result ? 0 : 1);
}


int __stdcall icap_step(int h, double* curStep_in)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;

    bool result = handle->Step(curStep_in, false);
    return (result ? 0 : 1);
}


int __stdcall icap_step_dt(int h, double dt, double* elapsedTime)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;

    bool result = handle->Step(elapsedTime, dt, false);
    return (result ? 0 : 1);
}


// enable real-time mode, where results can be set and queried
int __stdcall icap_enable_rt_mode(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;

    handle->EnableRealTimeStatus();
    
    return 0;
}


// disable real-time mode, and just execute from input file
int __stdcall icap_disable_rt_mode(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;

    return 0;
}


int __stdcall icap_clear_node_flows(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return ERROR_VAL;

    handle->InitializeZeroFlows();

    return 0;
}

int __stdcall icap_set_node_flow(int h, char* nodeId, double flow)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return ERROR_VAL;

    handle->SetCurrentNodeInflow(nodeId, flow);

    return 0;
}

int __stdcall icap_set_node_head(int h, char* nodeId, double head)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return ERROR_VAL;

    handle->SetCurrentNodeHead(nodeId, head);

    return 0;
}


double __stdcall icap_get_node_head(int h, char* nodeId)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return ERROR_VAL;

    return handle->GetCurrentNodeHead(nodeId);
}


double __stdcall icap_get_node_us_inflows(int h, char* nodeId)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return ERROR_VAL;

    return handle->GetCurrentNodeInflow(nodeId);
}


int __stdcall icap_add_source(int h, char* nodeId)
{
    ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return ERROR_VAL;

    handle->AddSource(nodeId);

    return 0;
}


} // extern "C"
