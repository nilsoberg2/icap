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
//	h = c.AutoCreateHPG(diameter, length, roughness, slope, dsInvert, nodeID, dsStation);
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
