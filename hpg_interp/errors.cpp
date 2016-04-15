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

#include "hpg.hpp"
#include "errors.hpp"
#include "impl.h"


namespace hpg
{

    std::string Hpg::getErrorMessage()
    {
        if (! impl->errorCode)
            return "";
        else if (impl->errorCode == err::InvalidFlow)
            return "Interpolation: Attempted to use invalid flow outside valid HPG flow range.";
        else if (impl->errorCode == err::InvalidInput)
            return "Interpolation: Attempted to use an input point outside the valid range.";
        else if (impl->errorCode == err::InvalidParam)
            return "An invalid parameter was given.";
        else if (impl->errorCode == err::GenericInterpFailed)
            return "Interpolation: An unknown failure occurred in interpolation.";
        else if (impl->errorCode == err::InvalidSplineSize)
            return "Splines: The number of elements are too few to create a spline(s) with.";
        else if (impl->errorCode == err::GenericSplineError)
            return "Splines: An unknown error occurred.";
        else if (impl->errorCode == err::FileReadFailed)
            return "Load: Couldn't read the given file.";
        else if (impl->errorCode == err::FileWriteFailed)
            return "Load: Couldn't write to the given file.";
        else if (impl->errorCode == err::InvalidFileFormat)
            return "Load: Invalid file format.";
        else if (impl->errorCode == err::CantComputeNormCrit)
            return "Interpolation: Can't compute normal or critical depth.";
        else if (impl->errorCode == error::divergence)
            return "Normal/Critical: Solution did not converge.";
        else if (impl->errorCode == error::imaginary)
            return "Normal/Critical: imaginary number encountered.";
        else
            return "Unknown error.";
    }

    int Hpg::getErrorCode()
    {
        return impl->errorCode;
    }

}
