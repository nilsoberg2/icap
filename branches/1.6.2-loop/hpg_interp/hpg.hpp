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
        //Destructor. Implemented in derived classes.
        ~Hpg();
        /** Load a HPG.  Optionally sets up the splines.
        * @param file          HPG file to load as string
        * @param setupSplines  Setup splines as boolean
        * @return true if successful, false otherwise
        */
        bool LoadFromFile(const char* file, bool splineSetup = true);
        /** Save a HPG to a file.
        * @param file HPG file to load as string
        * @return true if successful, false otherwise
        */
        bool SaveToFile(const char* file, bool append = false);

        // ACCESSOR FUNCTIONS

        unsigned int NumPosFlows();
        unsigned int NumAdvFlows();
        double PosFlowAt(unsigned int f);
        double AdvFlowAt(unsigned int f);
        hpgvec& PosValuesAt(unsigned int f);
        hpgvec& AdvValuesAt(unsigned int f);
        // Determine if curve is mild-slope.
        bool IsMildAt(unsigned int curve);
        // Determine if curve is steep-slope. negative = steep, positive = mild, 0 = error
        int IsSteepAt(unsigned int curve);
        // Add a curve to the HPG. Automatically determines if pos or adverse.
        void AddCurve(double flow, hpgvec& curve, point crit);

        // INTERPOLATION FUNCTIONS

        int GetUpstream(double flow, double downstream, double& result);
        int GetDownstreamExactFlow(double flow, double upstream, double& result);
		int GetVolume(double flow, double downstream, double& volume);
		int GetHf(double flow, double downstream, double& value);
        //int GetCritUpstream(double flow, double& result) = 0;
        //int GetCritDownstream(double flow, double& result) = 0;
        //int GetCritUpFromDown(double flow, double downstream, double& result);
        //int GetZeroUpstream(double downstream, double& result);
        //int GetZeroDownstream(double upstream, double& result);
        int GetLastPoint(double flow, point& result);
        int GetFlowIndex(double flow, unsigned int& index);
        int FindMatchingFlowIndex(double flow, unsigned int& index);
        bool IsValidFlow(double flow);
		int IsValidFlowExtended(double flow);
        int GetFirstPointOnCurve(double flow, unsigned int curve, point& result);
        int GetLastPointOnCurve(double flow, unsigned int curve, point& result);
        hpgvec& GetPosCritical();
        hpgvec& GetAdvCritical();

        // ERROR FUNCTIONS

        // Get the error as a string.  Returns the null string if no error.
        char* GetErrorStr();
        // Get the error code as an integer.
        int GetError();

        int SetupSplines();

    public:

        bool IsDSInvertValid();
        double GetDSInvert();
        void SetDSInvert(double dsinvert);
        bool IsUSInvertValid();
        double GetUSInvert();
        void SetUSInvert(double usinvert);
        bool IsDSStationValid();
        double GetDSStation();
        void SetDSStation(double dsstation);
        bool IsUSStationValid();
        double GetUSStation();
        void SetUSStation(double usstation);
        bool IsSlopeValid();
        double GetSlope();
        void SetSlope(double slope);
        bool IsLengthValid();
        double GetLength();
        void SetLength(double length);
        bool IsRoughnessValid();
        double GetRoughness();
        void SetRoughness(double roughness);
        
        //ATTRIBUTE ACCESSORS

        bool IsMaxDepthValid();
        double GetMaxDepth();
        void SetMaxDepth(double maxDepth);
        bool IsUnsteadyDepthPctValid();
        double GetUnsteadyDepthPct();
        void SetUnsteadyDepthPct(double maxdepth);
        int GetNodeID();
        void SetNodeID(int id);

    protected:
        void Initialize();
        void Deinitialize();
        void CopyInto(const Hpg& hpg);
        
        std::string SaveHeader();
        bool LoadHeader(std::ifstream& fh);
        void PostLoadActions();

        int standardInterpolation(unsigned int curve, double flow, double input, double& result, InterpValue interpAction = Interp_Upstream);
        int standardExtrapolation(unsigned int curve, double flow, double downstream, double& result);
        int interpolateSteepSpecial(unsigned int curve, double flow, double downstream, double& result);
		double linearInterpQ(double flow, double f1, double f2, double y1, double y2);
		double linearInterp(double x, double x1, double y1, double x2, double y2);
        int setupPosSplines();
        int setupAdvSplines();
        int setupCritPosSplines();
        int setupCritAdvSplines();
    private:
        class Impl;
        Impl* impl;
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
