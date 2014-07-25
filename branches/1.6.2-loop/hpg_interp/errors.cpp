#include "hpg.hpp"
#include "errors.hpp"
#include "impl.h"


namespace hpg
{

    char* Hpg::GetErrorStr()
    {
        if (! impl->ErrorCode)
            return "";
        else if (impl->ErrorCode == err::InvalidFlow)
            return "Interpolation: Attempted to use invalid flow outside valid HPG flow range.";
        else if (impl->ErrorCode == err::InvalidInput)
            return "Interpolation: Attempted to use an input point outside the valid range.";
        else if (impl->ErrorCode == err::InvalidParam)
            return "An invalid parameter was given.";
        else if (impl->ErrorCode == err::GenericInterpFailed)
            return "Interpolation: An unknown failure occurred in interpolation.";
        else if (impl->ErrorCode == err::InvalidSplineSize)
            return "Splines: The number of elements are too few to create a spline(s) with.";
        else if (impl->ErrorCode == err::GenericSplineError)
            return "Splines: An unknown error occurred.";
        else if (impl->ErrorCode == err::FileReadFailed)
            return "Load: Couldn't read the given file.";
        else if (impl->ErrorCode == err::FileWriteFailed)
            return "Load: Couldn't write to the given file.";
        else if (impl->ErrorCode == err::InvalidFileFormat)
            return "Load: Invalid file format.";
        else if (impl->ErrorCode == err::CantComputeNormCrit)
            return "Interpolation: Can't compute normal or critical depth.";
        else if (impl->ErrorCode == error::divergence)
            return "Normal/Critical: Solution did not converge.";
        else if (impl->ErrorCode == error::imaginary)
            return "Normal/Critical: imaginary number encountered.";
        else
            return "Unknown error.";
    }

    int Hpg::GetError()
    {
        return impl->ErrorCode;
    }

}
