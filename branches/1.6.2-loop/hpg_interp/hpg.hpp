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
