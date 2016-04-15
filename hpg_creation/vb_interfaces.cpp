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

//#if defined(_MSC_VER)
//
//#include <windows.h>
//#include <comdef.h>
//#include <tchar.h>
//#include <hpg.hpp>
//#include <hpg/units.hpp>
//#include <hpg/file.hpp>
//#include <hpg/exception.hpp>
//#include "hpg_creator.hpp"
//
//
//HpgCreator_API long hpgMetricUnits()
//{
//	return hpg::units::si;
//}
//
//
//HpgCreator_API long hpgEnglishUnits()
//{
//	return hpg::units::english;
//}
//
//HpgCreator_API long hpgAutoCreateHPG(double diameter,
//								double length,
//								double roughness,
//								double slope,
//								double dsInvert,
//								double unsteadyDepth,
//								long units,
//								char *outputFile,
//								long nodeID,
//								double dsStation)
//{
//	long retval = 0;
//
//	HpgCreator c;
//	c.setNumberOfCurves(50);
//	c.setMaxDepthFraction(unsteadyDepth);
//
//	try
//	{
//		c.setUnits(units);
//	}
//	catch (hpg::error::exception e)
//	{
//		return 1;
//	}
//
//    hpg::CircularHpg* h;
//	// create positive slope version
//	h = c.AutoCreateHpg(diameter, length, roughness, slope, dsInvert, nodeID, dsStation);
//
//	if (h == NULL)
//		retval = 1;
//	else if (! h->SaveToFile(outputFile))
//		retval = h->getError();
//	if (h != NULL)
//		delete h;
//
//	return retval;
//}
//
//#endif
