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
