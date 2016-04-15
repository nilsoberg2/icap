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


#include <cmath>


#ifdef ICAP_DEBUG
void OutputTimeStep(char* datetime, FILE* fh)
{
    fprintf(fh, "TIMESTEP: %s\n", datetime);
    
    for (int i = GLINK_COUNT-1; i >= 0; i--)
    {
        // Compute the maximum node depth.
        double maxDepth = GLINK_MAXDEPTH(i) * 0.80;
        
        // Output the link status.
        fprintf(fh, "%s\t%f\t%f\t%f\t", GLINK_ID(i), GLINK_FLOW(i), GLINK_DSDEPTH(i), GLINK_USDEPTH(i));
        
        // Determine if the pipe is pressurized.
        if (GLINK_USDEPTH(i) > maxDepth && GLINK_DSDEPTH(i) > maxDepth)
            fprintf(fh, "Pressurized");
        
        // Else, determine if part of the pipe is pressurized.
        else if ((GLINK_USDEPTH(i) > maxDepth && GLINK_DSDEPTH(i) <= maxDepth) ||
                 (GLINK_USDEPTH(i) <= maxDepth && GLINK_DSDEPTH(i) > maxDepth))
            fprintf(fh, "Transitional");
        
        // No-flow case
        else if (isZero(GLINK_FLOW(i)))
        {
            if (GLINK_USDEPTH(i) > 0.0)
                fprintf(fh, "Ponded");
            else
                fprintf(fh, "Empty");
        }

        // Final condition is open channel flow.
        else if (GLINK_USDEPTH(i) <= maxDepth && GLINK_DSDEPTH(i) <= maxDepth)
            fprintf(fh, "Open-channel");

        fprintf(fh, "\n");
    }
}
#endif
