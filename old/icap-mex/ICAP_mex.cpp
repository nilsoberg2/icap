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

#include "mex.h"
#include "class_handle.hpp"
#include <stdio.h>
#include "../icap/icap_interface.h"
#include "class_handle.hpp"
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/file.hpp>


mxArray* errorFunction(char* message)
{
    mexWarnMsgTxt(message);
    return mxCreateDoubleScalar(1);
}


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char cmd[25];
    if (nrhs < 1 || mxGetString(prhs[0], cmd, sizeof(cmd)))
    {
        mexErrMsgTxt("First input should be a command string less than 25 characters long.");
    }
    if (nlhs < 1)
    {
        mexErrMsgTxt("Always need at least one output argument.");
    }
    
    ///////////////////////////////////////////////////////////////////////
    // NEW
    if (!strcmp("new", cmd)) {
        // Check parameters
        if (nlhs != 1)
        {
            mexErrMsgTxt("New: One output expected.");
        }
        
        // Return a handle to a new C++ instance
        uint32_t handle = NewICAP();
        plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
        *((uint32_t*)mxGetData(plhs[0])) = handle;
        boost::log::add_file_log(
                boost::log::keywords::file_name = "icap-mex.log",
                boost::log::keywords::auto_flush = true
                );
        return;
    }
    
    // Check there is a second input, which should be the class instance handle
    if (nrhs < 2)
    {
        plhs[0] = errorFunction("Second input should be a class instance handle.");
        return;
    }
    
    // Get the handle
    uint32_t handle = *((uint32_t *)mxGetData(prhs[1]));
    
    ///////////////////////////////////////////////////////////////////////
    // DELETE
    if (!strcmp("delete", cmd)) {
        // Destroy the C++ object
        //destroyObject<uint32_t>(prhs[1]);
        mexUnlock();
        
        DeleteICAP(handle);
        plhs[0] = mxCreateDoubleScalar(0);
        return;
    }
    
    ///////////////////////////////////////////////////////////////////////
    // OPEN
    if (!strcmp("open", cmd))
    {
        if (nlhs < 1 || nrhs < 5)
        {
            plhs[0] = errorFunction("ICAP_open: Unexpected arguments (requires inputFile, reportFile, outputFile as arguments)");
            return;
        }
        char inputFile[256];
        char outputFile[256];
        char reportFile[256];
        if (mxGetString(prhs[2], inputFile, sizeof(inputFile)))
        {
            plhs[0] = errorFunction("ICAP_open: Invalid arguments provided, unable to convert inputFile argument to string.");
            return;
        }
        if (mxGetString(prhs[3], reportFile, sizeof(reportFile)))
        {
            plhs[0] = errorFunction("ICAP_open: Invalid arguments provided, unable to convert reportFile argument to string.");
            return;
        }
        if (mxGetString(prhs[4], outputFile, sizeof(outputFile)))
        {
            plhs[0] = errorFunction("ICAP_open: Invalid arguments provided, unable to convert outputFile argument to string.");
            return;
        }
        
        plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0]) = icap_open(handle, inputFile, reportFile, outputFile);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // START
    if (!strcmp("start", cmd))
    {
        if (nlhs < 1 || nrhs < 2)
        {
            plhs[0] = errorFunction("ICAP_start: Unexpected arguments");
            return;
        }
        plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0]) = icap_start(handle);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // END
    if (!strcmp("end", cmd))
    {
        if (nlhs < 1 || nrhs < 2)
        {
            plhs[0] = errorFunction("ICAP_end: Unexpected arguments");
            return;
        }
        plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0]) = icap_end(handle);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // CLOSE
    if (!strcmp("close", cmd))
    {
        if (nlhs < 1 || nrhs < 2)
        {
            plhs[0] = errorFunction("ICAP_close: Unexpected arguments");
            return;
        }
        plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0]) = icap_close(handle);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // ENABLE_RT_MODE
    if (!strcmp("enable_rt_mode", cmd))
    {
        if (nlhs < 1 || nrhs < 2)
        {
            plhs[0] = errorFunction("ICAP_enable_rt_mode: Unexpected arguments");
            return;
        }
        plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0]) = icap_enable_rt_mode(handle);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // SET_NODE_FLOW
    if (!strcmp("set_node_flow", cmd))
    {
        if (nlhs < 1 || nrhs < 4)
        {
            plhs[0] = errorFunction("ICAP_set_node_flow: Unexpected arguments (requires nodeId and flow)");
            return;
        }
        
        char nodeId[50];
        if (mxGetString(prhs[2], nodeId, sizeof(nodeId)))
        {
            plhs[0] = errorFunction("ICAP_set_node_flow: Invalid arguments provided, unable to convert to string.");
            return;
        }
        double flow = mxGetScalar(prhs[3]);
        
        plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0]) = 0;
        icap_set_node_flow(handle, nodeId, flow);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // SET_NODE_HEAD
    if (!strcmp("set_node_head", cmd))
    {
        if (nlhs < 1 || nrhs < 4)
        {
            plhs[0] = errorFunction("ICAP_set_node_head: Unexpected arguments (requires nodeId and head)");
            return;
        }
        
        char nodeId[50];
        if (mxGetString(prhs[2], nodeId, sizeof(nodeId)))
        {
            plhs[0] = errorFunction("ICAP_set_node_head: Invalid arguments provided, unable to convert to string.");
            return;
        }
        double head = mxGetScalar(prhs[3]);
        
        plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0]) = 0;
        //icap_set_node_head(handle, nodeId, head);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // GET_NODE_HEAD
    if (!strcmp("get_node_head", cmd))
    {
        if (nlhs < 2 || nrhs < 3)
        {
            plhs[0] = errorFunction("ICAP_get_node_head: Unexpected arguments (requires nodeId)");
            plhs[1] = mxCreateDoubleScalar(0);
            return;
        }
        
        char nodeId[50];
        if (mxGetString(prhs[2], nodeId, sizeof(nodeId)))
        {
            plhs[0] = errorFunction("ICAP_get_node_head: Invalid arguments provided, unable to convert to string.");
            plhs[1] = mxCreateDoubleScalar(0);
            return;
        }
        
        plhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[1]) = icap_get_node_head(handle, nodeId);
        plhs[0] = mxCreateDoubleScalar(0);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // GET_NODE_US_INFLOWS
    if (!strcmp("get_node_us_inflows", cmd))
    {
        if (nlhs < 2 || nrhs < 3)
        {
            plhs[0] = errorFunction("get_node_us_inflows: Unexpected arguments (requires nodeId)");
            plhs[1] = mxCreateDoubleScalar(0);
            return;
        }
        
        char nodeId[50];
        if (mxGetString(prhs[2], nodeId, sizeof(nodeId)))
        {
            plhs[0] = errorFunction("get_node_us_inflows: Invalid arguments provided, unable to convert to string.");
            plhs[1] = mxCreateDoubleScalar(0);
            return;
        }
        
        plhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[1]) = icap_get_node_us_inflows(handle, nodeId);
        plhs[0] = mxCreateDoubleScalar(0);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // STEP
    if (!strcmp("step", cmd))
    {
        if (nlhs < 2 || nrhs < 3)
        {
            plhs[0] = errorFunction("ICAP_step: Unexpected arguments (requires two outputs and three inputs)");
            return;
        }
        
        plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
        plhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
        double dt = mxGetScalar(prhs[2]);
        //*mxGetPr(plhs[0]) = icap_step_dt(handle, dt);
        *mxGetPr(plhs[0]) = icap_step(handle, &dt);
        *mxGetPr(plhs[1]) = dt;
    }
    
    ///////////////////////////////////////////////////////////////////////
    // CLEAR_NODE_FLOWS
    if (!strcmp("clear_node_flows", cmd))
    {
        if (nlhs < 1 || nrhs < 2)
        {
            plhs[0] = errorFunction("ICAP_clear_node_flows: Unexpected arguments");
            return;
        }
        plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0]) = icap_clear_node_flows(handle);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // ADD_SOURCE
    if (!strcmp("add_source", cmd))
    {
        if (nlhs < 1 || nrhs < 3)
        {
            plhs[0] = errorFunction("ICAP_add_source: Unexpected arguments (requires nodeId)");
            return;
        }
        
        char nodeId[50];
        if (mxGetString(prhs[2], nodeId, sizeof(nodeId)))
        {
            plhs[0] = errorFunction("ICAP_add_source: Invalid arguments provided, unable to convert to string.");
            return;
        }
        
        plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0]) = icap_add_source(handle, nodeId);
    }
}

