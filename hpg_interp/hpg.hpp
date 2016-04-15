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
// 	Blake J. Landry, PhD
// 	Arthur R. Schmidt, PhD
// 	Ven Te Chow Hydrosystems Lab
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

/** @file 
* This contains the definitions for the base HPG class.
*/
#ifndef HPG_HPP___________________________20040831173030__
#define HPG_HPP___________________________20040831173030__

#include "../api.h"

#include "point.h"
#include "types.h"


// Begin wrapping code in the HPG namespace
namespace hpg
{

    /**
    * A class for reading, writing, and interpolating HPGs.
    * It provides the capability to read in a HPG file, write out
    * a HPG file, and get any upstream value from a (flow,
    * downstream) pair.
    *
    * Interpolation splines (functions) are created when the file
    * is loaded.
    */
    class Hpg
    {
    public:
        Hpg();

        //Destructor.
        ~Hpg();

        /** Load a HPG.  Optionally sets up the splines.
        * @param file          HPG file to load as string
        * @param setupSplines  Setup splines as boolean
        * @return true if successful, false otherwise
        */
        bool LoadFromFile(const std::string& path, bool splineSetup = true);
        /** Save a HPG to a file.
        * @param file HPG file to load as string
        * @return true if successful, false otherwise
        */
        bool SaveToFile(const std::string& path, bool append = false);

        // ACCESSOR FUNCTIONS

        //unsigned int NumPosFlows();
        //unsigned int NumAdvFlows();
        //hpgvec& PosValuesAt(unsigned int f);
        //hpgvec& AdvValuesAt(unsigned int f);
        // Determine if curve is mild-slope.
        //bool IsMildAt(unsigned int curve);
        // Add a curve to the HPG. Automatically determines if pos or adverse.
        void AddCurve(double flow, hpgvec& curve, point crit);

        // INTERPOLATION FUNCTIONS

        int InterpUpstreamHead(double flow, double downstream, double& result);
		int InterpVolume(double flow, double downstream, double& volume);
		int InterpHf(double flow, double downstream, double& value);
        //int GetCritUpstream(double flow, double& result) = 0;
        //int GetCritDownstream(double flow, double& result) = 0;
        //int GetCritUpFromDown(double flow, double downstream, double& result);
        //int GetZeroUpstream(double downstream, double& result);
        //int GetZeroDownstream(double upstream, double& result);

        // ERROR FUNCTIONS

        // Get the error as a string.  Returns the null string if no error.
        std::string getErrorMessage();
        // Get the error code as an integer.
        int getErrorCode();

    public:

        bool isDsInvertValid();
        double getDsInvert();
        void setDsInvert(double dsinvert);
        bool isUsInvertValid();
        double getUsInvert();
        void setUsInvert(double usinvert);
        bool isDsStationValid();
        double getDsStation();
        void setDsStation(double dsstation);
        bool isUsStationValid();
        double getUsStation();
        void setUsStation(double usstation);
        bool isLengthValid();
        double getLength();
        void setLength(double length);
        bool isRoughnessValid();
        double getRoughness();
        void setRoughness(double roughness);
        bool isMaxDepthValid();
        double getMaxDepth();
        void setMaxDepth(double maxDepth);
        bool isMaxDepthFractionValid();
        double getMaxDepthFraction();
        void setMaxDepthFraction(double maxdepth);
        std::string getNodeId();
        void setNodeId(std::string id);

        // Returns -1 if the HPG is not versioned, otherwise returns > 0
        int getVersion();

    private:
        class Impl;
        Impl* impl;


    private:
        //double PosFlowAt(unsigned int f);
        //double AdvFlowAt(unsigned int f);

        // Determine if curve is steep-slope. negative = steep, positive = mild, 0 = error
        bool isCurveSteep(unsigned int curve);
        //int GetLastPoint(double flow, point& result);
        int findLowerBracketingCurve(double flow, unsigned int& index);
        //int FindMatchingFlowIndex(double flow, unsigned int& index);
        bool isValidFlow(double flow);
		int isValidFlowExtended(double flow);
        int getFirstPointOnCurve(double flow, unsigned int curve, point& result);
        int getLastPointOnCurve(double flow, unsigned int curve, point& result);
        //hpgvec& GetPosCritical();
        //hpgvec& GetAdvCritical();
        int setupSplines();
        void initialize();
        void deinitialize();
        void copyFrom(const Hpg& hpg);
        
        std::string saveHeader();
        bool loadHeader(std::ifstream& fh);
        //void PostLoadActions();

        int standardInterpolation(unsigned int curve, double flow, double input, double& result, InterpValue interpAction = Interp_Upstream);
        int standardExtrapolation(unsigned int curve, double flow, double downstream, double& result);
        int interpolateSteepSpecial(unsigned int curve, double flow, double downstream, double& result);
		double linearInterpQ(double flow, double f1, double f2, double y1, double y2);
		double linearInterp(double x, double x1, double y1, double x2, double y2);
        int setupPosSplines();
        int setupAdvSplines();
        int setupCritPosSplines();
        int setupCritAdvSplines();

    };

    const point NULL_POINT;

    bool point_ax_lt_bx(const point& a, const point& b);
    bool point_ay_lt_by(const point& a, const point& b);
    bool point_av_lt_bv(const point& a, const point& b);
    bool point_ax_gt_bx(const point& a, const point& b);
    bool point_ay_gt_by(const point& a, const point& b);
    bool point_av_gt_bv(const point& a, const point& b);

}//end namespace hpg2


#endif//HPG_HPP___________________________20040831173030__
