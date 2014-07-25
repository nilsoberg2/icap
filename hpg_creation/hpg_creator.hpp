#ifndef HPG_CREATOR_H_____________________20041020122121__
#define HPG_CREATOR_H_____________________20041020122121__


#include <deque>

#include "../hpg_interp/hpg.hpp"
#include "../hpg/error.hpp"
#include "../xslib/reach.h"


/** @file
* This file is the header file for the HPG Creator library.
*/
//
//
//// Support Win32 and GNU GCC library export.  Win32 must
//// explicitly specify what is exported.
//#if defined(_MSC_VER)
//#if defined(HpgCreator_EXPORTS)
//#define HpgCreator_API __declspec(dllexport)
//#else
//#define HpgCreator_API __declspec(dllimport)
//#endif
//#else
//#define HpgCreator_API
//#endif


class HpgCreator
{
private:
    /**
    * This section describes computation characteristics.
    */
    int numSteps; /**< number of computations in back-water curve computaiton */
    double convergenceTol; /**< convergence factor determines if iterative solution has converged; defaults to 0.000001 */
    int maxIterations; /**< maximum number of iterations when solving; defaults to 100 */
    double maxDepthFrac; /**< maximum depth (% of diameter) before cutting off computations; defaults to 0.80 */
    int numHpc; /**< maximum number of curves in HPG; defaults to 20 */
    int numPoints; /**< maximum number of points on each curve; defaults to 40 */
    double g; /**< gravity constant. changes when changing units; defaults to 9.81 (SI) 32.486 (English) */
    double kn; /**< units constant. changes when changing units; defaults to 1.00 (SI) 1.486 (English) */
    short units; /**< units specifier; defaults to English units */
    int errorCode; /**< error code; 0 == no error, non-0 == error */
    int minCurvePoints; /**< this is the minimum number of points on a curve that are required */
public:
    /**
    * Constructor initializes everything to default values.
    */
    HpgCreator();

    std::shared_ptr<hpg::Hpg> AutoCreateHPG(const xs::Reach& reach);

    double FindMaxFlow(const xs::Reach& reach, bool reverseSlope, double &yCritMax);
    void FindFlowIncrements(const xs::Reach& reach, bool reverseSlope, double minFlow, double maxFlow, std::deque<double> &flows);

private:
    //HPG * AutoCreateHPG2(double diameter, double length, double roughness, double slope, double dsInvert, long nodeID = -1, double dsStation = hpg::error::bad_value);

    //double FindCriticalFlow(const xs::Reach& reach, double depth);
    //double FindMinFlow(const xs::Reach& reach, bool reverseSlope, double starting_discharge, double ending_discharge, double &y_critical_min);

    void ComputeHPGCurve(const xs::Reach& reach, double flow, double pressurizedHeight, bool reverseSlope, double& yNormal, double& yCritical, hpg::hpgvec &curve);
    bool ComputeValidHPGCurve(const xs::Reach& reach, double flow, double pressurizedHeight, bool reverseSlope, double& yNormal, double& yCritical, hpg::hpgvec &curve);

    /**
    * getConvergenceTolerance returns the convergence factor for
    * creating HPGs.
    */
    double getConvergenceTolerance();
    /**
    * setConvergenceTolerance sets the convergence factor for
    * creating HPGs.
    */
    void setConvergenceTolerance(double factor);
    /**
    * getNumberOfCurves gets the maximum number of curves to
    * create in an HPG.
    */
    int getNumberOfCurves();
    /**
    * setNumberOfCurves sets the maximum number of curves to
    * create in an HPG.
    */
    void setNumberOfCurves(int curves);
    /**
    * getNumberOfPointsPerCurve gets the maximum number of
    * points to use on a curve when creating HPGs.
    */
    int getNumberOfPointsPerCurve();
    /**
    * setNumberOfPointsPerCurve sets the maximum number of
    * points to use on a curve when creating HPGs.
    */
    void setNumberOfPointsPerCurve(int points);
    /**
    * getMaxDepthFraction gets the depth as a percentage that
    * is considered to be unsteady flow.  Value should be a
    * number between and including 0 and 1.
    */
    double getMaxDepthFraction();
    /**
    * setMaxDepthFraction sets the depth as a percentage that
    * is considered to be unsteady flow.  Value should be a
    * number between and including 0 and 1.
    */
    void setMaxDepthFraction(double depth);
    /**
    * getMaxIterations returns the maximum number of
    * iterations allowed during computations.
    */
    int getMaxIterations();
    /**
    * setMaxIterations sets the maximum number of
    * iterations allowed during computations.
    */
    void setMaxIterations(int iterations);
    /**
    * setUnits sets the units type for this HPG. Valid types are
    * specified in <tarp/units.h> header.
    */
    void setUnits(int u);
    /**
    * getMinCurveSize returns the minimum number of points that constitute
    * a valid curve.
    */
    int getMinCurveSize();
    /**
    * setMinCurveSize sets the minimum number of points that are required 
    * to constitute a valid curve.
    */
    void setMinCurveSize(int size);

    int getNumBackwaterSteps();
    void setNumBackwaterSteps(int numComp);

    /**
    * Return the error code.
    */
    int getError();
    //
    //
    //
};


enum HpgUnits
{
    Hpg_SI,
    Hpg_English,
};

//#if defined(_MSC_VER)
//HPGCREATOR_API long hpgMetricUnits();
//HPGCREATOR_API long hpgEnglishUnits();
//HPGCREATOR_API long hpgAutoCreateHPG(double diameter,
//                                     double length,
//                                     double roughness,
//                                     double slope,
//                                     double dsInvert,
//                                     double unsteadyDepth,
//                                     long units,
//                                     char *outputFile,
//                                     long nodeID,
//                                     double dsStation);
//#endif

#endif//HPG_CREATOR_H_____________________20041020122121__
