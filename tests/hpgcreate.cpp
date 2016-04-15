#include "stdafx.h"
#include "CppUnitTest.h"
#include <Windows.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>

#include "../hpg_creation/hpg_creator.hpp"
#include "../hpg_creation/profile.h"
#include "../hpg_creation/normcrit.h"
#include "../hpg_interp/hpg.hpp"
#include "../xslib/circular.h"
#include "../util/math.h"
#include "../bspline/BSpline/BSpline.h"
//#define NORMCRIT_STATIC
//#include "../../../../hpg/oberg/cpp/normcrit/trunk/normcrit.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = boost::filesystem;

const double kn = 1.485918577496261;
const double g = 32.1740;// 32.185039370078741;
char* hpgPath = "..\\test\\hpg.test.txt";

namespace TestGeometryRead
{		
	TEST_CLASS(HpgCreateTest)
	{
	public:

        HpgCreateTest()
        {
            data = new double[80 * 4] {
                373.427500, 376.730000, 2.505274, 0.000000000000,
                373.767813, 376.730000, 58.747975, 0.000000000000,
                374.108125, 376.730000, 225.857279, 0.000000000000,
                374.448438, 376.730000, 537.564517, 0.000000000000,
                374.788750, 376.730000, 1019.506076, 0.000000000000,
                375.129063, 376.730000, 1692.664791, 0.000000000000,
                375.469375, 376.730000, 2574.808548, 0.000000000000,
                375.809688, 376.730000, 3681.266294, 0.000000000000,
                376.150000, 376.730000, 5025.404945, 0.000000000000,
                376.490313, 376.730000, 6618.947307, 0.000000000000,
                376.830625, 376.830625, 8470.970837, 0.000000000000,
                377.170938, 377.170938, 10545.229959, 0.000000000000,
                377.511250, 377.511250, 12789.358247, 0.000000000000,
                377.851563, 377.851563, 15175.298746, 0.000000000000,
                378.191875, 378.191875, 17682.743230, 0.000000000000,
                378.532188, 378.532188, 20295.340594, 0.000000000000,
                378.872500, 378.872500, 22999.197914, 0.000000000000,
                379.212813, 379.212813, 25782.097085, 0.000000000000,
                379.553125, 379.553125, 28633.023650, 0.000000000000,
                379.893438, 379.893438, 31541.856656, 0.000000000000,
                380.233750, 380.233750, 34499.150613, 0.000000000000,
                380.574063, 380.574063, 37495.973923, 0.000000000000,
                380.914375, 380.914375, 40523.783550, 0.000000000000,
                381.254688, 381.254688, 43574.323598, 0.000000000000,
                381.595000, 381.595000, 46639.539746, 0.000000000000,
                381.935313, 381.935313, 49711.503968, 0.000000000000,
                382.275625, 382.275625, 52782.345320, 0.000000000000,
                382.615938, 382.615938, 55844.183403, 0.000000000000,
                382.956250, 382.956250, 58889.061382, 0.000000000000,
                383.296562, 383.296563, 61908.875416, 0.000000000000,
                383.636875, 383.636875, 64895.296914, 0.000000000000,
                383.977188, 383.977188, 67839.683092, 0.000000000000,
                384.317500, 384.317500, 70732.969695, 0.000000000000,
                384.657813, 384.657813, 73565.536827, 0.000000000000,
                384.998125, 384.998125, 76327.033799, 0.000000000000,
                385.338438, 385.338438, 79006.139406, 0.000000000000,
                385.678750, 385.678750, 81590.215055, 0.000000000000,
                386.019063, 386.019063, 84064.765792, 0.000000000000,
                386.359375, 386.359375, 86412.513996, 0.000000000000,
                386.699687, 386.699688, 88611.521735, 0.000000000000,
                387.380313, 387.380313, 92407.714675, 0.000000000000,
                412.720625, 412.720625, 98644.174884, 0.000000000000,
                438.060938, 438.060938, 98644.174884, 0.000000000000,
                463.401250, 463.401250, 98644.174884, 0.000000000000,
                488.741562, 488.741563, 98644.174884, 0.000000000000,
                514.081875, 514.081875, 98644.174884, 0.000000000000,
                539.422188, 539.422188, 98644.174884, 0.000000000000,
                564.762500, 564.762500, 98644.174884, 0.000000000000,
                590.102813, 590.102813, 98644.174884, 0.000000000000,
                615.443125, 615.443125, 98644.174884, 0.000000000000,
                640.783437, 640.783438, 98644.174884, 0.000000000000,
                666.123750, 666.123750, 98644.174884, 0.000000000000,
                691.464062, 691.464063, 98644.174884, 0.000000000000,
                716.804375, 716.804375, 98644.174884, 0.000000000000,
                742.144687, 742.144688, 98644.174884, 0.000000000000,
                767.485000, 767.485000, 98644.174884, 0.000000000000,
                792.825312, 792.825313, 98644.174884, 0.000000000000,
                818.165625, 818.165625, 98644.174884, 0.000000000000,
                843.505937, 843.505937, 98644.174884, 0.000000000000,
                868.846250, 868.846250, 98644.174884, 0.000000000000,
                894.186562, 894.186562, 98644.174884, 0.000000000000,
                919.526875, 919.526875, 98644.174884, 0.000000000000,
                944.867187, 944.867187, 98644.174884, 0.000000000000,
                970.207500, 970.207500, 98644.174884, 0.000000000000,
                995.547812, 995.547812, 98644.174884, 0.000000000000,
                1020.888125, 1020.888125, 98644.174884, 0.000000000000,
                1046.228437, 1046.228437, 98644.174884, 0.000000000000,
                1071.568750, 1071.568750, 98644.174884, 0.000000000000,
                1096.909062, 1096.909062, 98644.174884, 0.000000000000,
                1122.249375, 1122.249375, 98644.174884, 0.000000000000,
                1147.589687, 1147.589687, 98644.174884, 0.000000000000,
                1172.930000, 1172.930000, 98644.174884, 0.000000000000,
                1198.270312, 1198.270312, 98644.174884, 0.000000000000,
                1223.610625, 1223.610625, 98644.174884, 0.000000000000,
                1248.950937, 1248.950937, 98644.174884, 0.000000000000,
                1274.291250, 1274.291250, 98644.174884, 0.000000000000,
                1299.631562, 1299.631562, 98644.174884, 0.000000000000,
                1324.971875, 1324.971875, 98644.174884, 0.000000000000,
                1350.312187, 1350.312187, 98644.174884, 0.000000000000,
                1375.652500, 1375.652500, 98644.174884, 0.000000000000,
            };
        }

        static bool idComp(const std::string& id1, const std::string& id2)
        {
            using namespace std;
            cout << id1 << " " << id2 << endl;
            return id1 == id2;
        }

        xs::Reach makeReach(double usInvert, double length, double diameter = 10)
        {
            xs::Reach reach;
            reach.setLength(length);
            reach.setRoughness(0.015);
            reach.setDsInvert(0);
            reach.setUsInvert(usInvert);
            reach.setXs(std::shared_ptr<xs::CrossSection>(new xs::Circular(diameter)));
            return reach;
        }

        //void testReach(xs::Reach& reach, double* Qlist, double* targetNormalList, double* targetCriticalList, int nP);
        //void testReach1();
        //void testReach2();
        //void testReach3();

		TEST_METHOD(NormalCriticalTest)
		{
            using namespace std;
            bool status;

            double realYn = 0.522926705597983;

            double q = 6.4479441556280959;
            xs::Reach reach = makeReach(3.44, 664.318, 13.75);
            double yN;
            int error = ComputeNormalDepth(reach, q, g, kn, yN);
            //Assert::IsTrue(isZero(yN - realYn, 1e20), L"Failed normal depth");
            Assert::AreEqual(yN, realYn, L"Failed normal depth");

            //testReach1();
            //testReach2();
            //testReach3();
        }

        void testReach1()
        {
            xs::Reach reach = makeReach(1, 500);
            double Qlist[] = { 100, 500, 689 };
            double targetNormalList[] = { 2.670338254341911, 6.643752353743081, 9.287130811809327 };
            double targetCriticalList[] = { 2.316908799486291, 5.336763503940410, 6.309510810097808 };
            testReach(reach, Qlist, targetNormalList, targetCriticalList, 3);
        }

        void testReach2()
        {
            xs::Reach reach = makeReach(0.1, 500);
            double Qlist[] = { 50, 100, 217.9 };
            double targetNormalList[] = { 3.383980691831928, 4.961074314688030, 9.294019245616905 };
            double targetCriticalList[] = { 1.626682441395198, 2.316908799486291, 3.460513006870239 };
            testReach(reach, Qlist, targetNormalList, targetCriticalList, 3);
        }

        void testReach3()
        {
            xs::Reach reach = makeReach(1, 500);
            double Qlist[] = { 100, 500, 689 };
            double targetNormalList[] = { 2.670338254341911, 6.643752353743081, 9.287130811809327 };
            double targetCriticalList[] = { 2.316908799486291, 5.336763503940410, 6.309510810097808 };
            testReach(reach, Qlist, targetNormalList, targetCriticalList, 3);
        }

        void testReach(xs::Reach& reach, double* Qlist, double* targetNormalList, double* targetCriticalList, int nP)
        {
            double yN, yC;

            int error = 0;

            for (int i = 0; i < nP; i++)
            {
                error = ComputeNormalDepth(reach, Qlist[i], g, kn, yN);

                if (error)
                {
                    Assert::Fail(makeInfo(L"Failed normal depth: convergence error for Q=", std::to_string(Qlist[i])).c_str());
                }

                if (!isZero(yN - targetNormalList[i], 10e10))
                {
                    Assert::Fail(makeInfo(L"Failed normal depth: yN should be ", std::to_string(targetNormalList[i]) + " but is " + std::to_string(yN)).c_str());
                }

                error = ComputeCriticalDepth(reach, Qlist[i], g, yC);

                if (!isZero(yC - targetCriticalList[i], 10e10))
                {
                    Assert::Fail(makeInfo(L"Failed normal depth: yC should be ", std::to_string(targetCriticalList[i]) + " but is " + std::to_string(yC)).c_str());
                }
            }
		}

#define round(x) floor(x+0.5)
#define DBOUT( s )            \
        {                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringA( os_.str().c_str() );  \
        }

        TEST_METHOD(DetermineSteepStartingDepthTest)
        {
            using namespace std;
            bool status;

            double Q = 668.7;
            double dsInv = 373.29;
            double yNormal;
            int numSteps = 1000;

            xs::Reach reach = makeReach(376.73, 664.318, 13.75);
            reach.setDsInvert(dsInv);

            int errorCode = ComputeNormalDepth(reach, Q, g, kn, yNormal);
            //Assert::AreEqual(4.951167, yN);
            Assert::IsTrue(fabs(yNormal - 4.951167) < 1e-6, L"Bad normal");

            double yMin = yNormal;
            // Find the point at which the downstream starts to influence the upstream (e.g. there
            // is no change from S1 profile to S2 [HJ present]).
            double yInit = yNormal;
            double dy = 0.025; // small dy to find the point we are trying to find
            double yComp = yNormal;
            double yDlast = yNormal;
            double volume = 0.0;
            double hf_reach = 0;
            // We iterate until we've reached or exceeded the maximum depth.
            int count = 0;
            while (std::abs(yComp - yNormal) <= dy)
            {
                yDlast = yInit;
                // Go to the next depth.
                yInit = yInit + dy;

                errorCode = ComputeCombinedProfile(reach, Q, yInit, numSteps, false, false, g, kn, 1.0, yComp, volume, hf_reach);

                // If the solution went imaginary, did not converge, or reached
                // the maximum pipe depth and not enough points were found, then
                // terminate early.  If there was another error (at_min_depth)
                // then continue to the next higher depth.
                if (errorCode)
                {
                    if (errorCode == hpg::error::imaginary)
                        break;
                    else if (errorCode == hpg::error::divergence)
                        break;
                    else if (errorCode == hpg::error::at_max_depth && count < 4)
                        break;
                }
            }

            yMin = yDlast;
            errorCode = ComputeCombinedProfile(reach, Q, yMin, numSteps, false, false, g, kn, 1.0, yComp, volume, hf_reach);

            Assert::AreEqual(0, errorCode, L"There was an errror");

            Assert::AreEqual(yNormal, yComp);
        }

        TEST_METHOD(ComputeCurveTest)
        {
            using namespace std;
            bool status;

            double Q = 668.7;

            double dsInv = 373.29;
            xs::Reach reach = makeReach(376.73, 664.318, 13.75);
            reach.setDsInvert(dsInv);

            double yN;
            int error = ComputeNormalDepth(reach, Q, g, kn, yN);
            //Assert::AreEqual(4.951167, yN);
            Assert::IsTrue(fabs(yN - 4.951167) < 1e-6, L"Bad normal");

            int nC = 1000;
            double maxDepthFrac = 1;
            double yUp, volume, hf;
            double yInit = 9.143661;
            if (ComputeCombinedProfile(reach, Q, yInit, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for q=6.4 on steep reach");
            }

            //double Q = 2629.2;

            //double dsInv = 373.29;
            //xs::Reach reach = makeReach(376.73, 664.318, 13.75);
            //reach.setDsInvert(dsInv);

            //double yN;
            //int error = ComputeNormalDepth(reach, Q, g, kn, yN);
            ////Assert::AreEqual(0.521109, yN);
            ////Assert::IsTrue(fabs(yN - 0.522926705597983) < 1e-10, L"Bad normal");

            //int nC = 1000;
            //double maxDepthFrac = 1;
            //double yUp, volume, hf;
            //double yInit = 11.338342;
            //if (ComputeCombinedProfile(reach, Q, yInit, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            //{
            //    Assert::Fail(L"Failed to converge to solution for q=6.4 on steep reach");
            //}

            //double Q = 6.4479441556280959;

            //double dsInv = 373.29;
            //xs::Reach reach = makeReach(376.73, 664.318, 13.75);
            //reach.setDsInvert(dsInv);


            //double yN;
            //int error = ComputeNormalDepth(reach, Q, g, kn, yN);
            ////Assert::AreEqual(0.521109, yN);
            //Assert::IsTrue(fabs(yN - 0.522926705597983) < 1e-10, L"Bad normal");

            //int nC = 1000;
            //double maxDepthFrac = 1;
            //double yUp, volume, hf;
            //double yInit = 1.9240159294823829;
            //if (ComputeCombinedProfile(reach, Q, yInit, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            //{
            //    Assert::Fail(L"Failed to converge to solution for q=6.4 on steep reach");
            //}

            //Assert::AreEqual(yN, yUp);
            //Assert::IsTrue(fabs(yUp - 0.521109) < 1e-5);

            //double yUp2;
            //if (ComputeCombinedProfile(reach, 6.4, yInit + 0.5, nC, false, false, g, kn, maxDepthFrac, yUp2, volume, hf))
            //{
            //    Assert::Fail(L"Failed to converge to solution for q=6.4 on steep reach");
            //}

            //hpg::hpgvec curve;
            //double yn, yc;
            //HpgCreator cc;
            //bool result = cc.computeValidHpgCurve(reach, 6.4, 1000, false, yn, yc, curve);

            //Assert::AreEqual(373.812913, curve.at(0).x);
        }

		TEST_METHOD(ComputeProfileTest)
		{
            using namespace std;
            bool status;

            xs::Reach reach = makeReach(1, 500);

            double volume = 0;
            double hf = 0;
            int error = 0;
            double yUp = 0;

            int nC = 500*0.3048;
            double fac = 100.;
            double maxDepthFrac = 1.;

            if (ComputeCombinedProfile(reach, 500, 10, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for full 500,10");
            }
            //Assert::AreEqual(round(yUp*fac), round(9.524223323566060*fac), makeInfo(L"Failed full-ds test: yUp should be 9.524223323566060 but is ", std::to_string(yUp)).c_str());

            if (ComputeCombinedProfile(reach, 1000., 0, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for empty fac,0");
            }
            Assert::AreEqual(round(yUp*fac), round(10.083769115971373*fac), makeInfo(L"Failed ds-empty test: yUp should be 10.083769115971373 but is ", std::to_string(yUp)).c_str());

            if (ComputeCombinedProfile(reach, 500, 8, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for 500,5");
            }
            Assert::AreEqual(round(7.578609087468569*fac), round(yUp*fac), makeInfo(L"Failed open test: yUp should be 7.578609087468569 but is ", std::to_string(yUp)).c_str());

            if (ComputeCombinedProfile(reach, 1000., 12, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for fac,12");
            }
            Assert::AreEqual(round(13.434423078981334*fac), round(yUp*fac), makeInfo(L"Failed full pressurized test: yUp should be 13.434423078981334 but is ", std::to_string(yUp)).c_str());

            if (ComputeCombinedProfile(reach, 0, 8, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for 0,8");
            }
            Assert::AreEqual(round(7*fac), round(yUp*fac), makeInfo(L"Failed zero test: yUp should be 7 but is ", std::to_string(yUp)).c_str());

            if (ComputeCombinedProfile(reach, 0, 12, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for 0,12");
            }
            Assert::AreEqual(round(11*fac), round(yUp*fac), makeInfo(L"Failed zero+full test: yUp should be 11 but is ", std::to_string(yUp)).c_str());

            xs::Reach advReach(reach);
            advReach.setDsInvert(reach.getUsInvert());
            advReach.setUsInvert(reach.getDsInvert());
            
            if (ComputeCombinedProfile(advReach, 500, 5, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for adverse 500,5");
            }
            Assert::AreEqual(round(8.528978419201232*fac), round(yUp*fac), makeInfo(L"Failed adverse test: yUp should be 8.528978419201232 but is ", std::to_string(yUp)).c_str());
            
            if (ComputeCombinedProfile(reach, 500, 5, nC, false, true, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for adverse 500,5 slopeRev=true");
            }
            Assert::AreEqual(round(8.528978419201232*fac), round(yUp*fac), makeInfo(L"Failed adverse test: yUp should be 8.528978419201232 but is ", std::to_string(yUp)).c_str());

            if (ComputeCombinedProfile(advReach, 5000, 5, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for 0,12");
            }
            Assert::AreEqual(round(71.852051313482434*fac), round(yUp*fac), makeInfo(L"Failed adverse test: yUp should be 71.852051313482434 but is ", std::to_string(yUp)).c_str());

            xs::Reach reach2;
            reach2.setLength(1364.79);
            reach2.setRoughness(0.015);
            reach2.setDsInvert(382.59);
            reach2.setUsInvert(386);
            reach2.setXs(std::shared_ptr<xs::CrossSection>(new xs::Circular(13.75)));

            //if (ComputeCombinedProfile(reach2, 1031.52, 393.067284, nC, false, true, g, kn, maxDepthFrac, yUp, volume, hf))
            //{
            //    Assert::Fail(L"Failed to converge to solution for 0,12");
            //}
            //Assert::AreEqual(round(391.263071*fac), round(yUp*fac), makeInfo(L"Failed adverse test: yUp should be 391.263071 but is ", std::to_string(yUp)).c_str());
            
            if (ComputeCombinedProfile(reach2, 1031.52, 7.067284, nC, false, true, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for 1031.52,7.067284");
            }
            Assert::AreEqual(round((14.040562010087399)*fac), round(yUp*fac), makeInfo(L"Failed adverse test: yUp should be 14.040562010087399 but is ", std::to_string(yUp)).c_str());

  
            ofstream output("zipiday.txt");
            for (double q = 1500; q <= 1900; q += 10)
            {
                for (double yd = 12; yd <= 15; yd += 0.01)
                {
                    ComputeCombinedProfile(reach2, q, yd, nC, false, true, g, kn, maxDepthFrac, yUp, volume, hf);
                    output << q << "\t" << yd << "\t" << yUp << endl;
                }
            }
            output.close();

            

            xs::Reach reach3;
            reach3.setLength(1364.79);
            reach3.setRoughness(0.015);
            reach3.setDsInvert(0);
            reach3.setUsInvert(3.41);
            reach3.setXs(std::shared_ptr<xs::CrossSection>(new xs::Circular(13.75)));
            HpgCreator c;
            std::shared_ptr<hpg::Hpg> hpgTemp = c.AutoCreateHpg(reach3);
            hpgTemp->SaveToFile("trialX.txt");
            
  
            //ofstream output("zipiday.txt");
            //for (double q = 1500; q <= 1900; q += 10)
            //{
            //    for (double yd = 12; yd <= 15; yd += 0.01)
            //    {
            //        ComputeCombinedProfile(reach2, q, yd, nC, false, true, g, kn, maxDepthFrac, yUp, volume, hf);
            //        output << q << "\t" << yd << "\t" << yUp << endl;
            //    }
            //}
            //output.close();
		}
        
        void hpgInit()
        {
            if (!boost::filesystem::exists(hpgPath))
            {
                xs::Reach reach = makeReach(2.66, 1530);

                HpgCreator c;
                std::shared_ptr<hpg::Hpg> hpgTemp = c.AutoCreateHpg(reach);
                hpgTemp->SaveToFile(hpgPath);
            }
        }

        struct interp
        {
            interp(double q, double i, double ex) : flow(q), in(i), expected(ex) {}
            double flow;
            double in;
            double out;
            double expected;
        };
        
		TEST_METHOD(InitTest)
		{
            using namespace std;
            bool status;

            hpgInit();
        }

		TEST_METHOD(InterpolateHpgTest)
		{
            using namespace std;
            bool status;

            hpgInit();

            hpg::Hpg hpg;
            Assert::IsTrue(hpg.LoadFromFile(hpgPath), makeInfo(L"Failed to load HPG: ", hpg.getErrorMessage()).c_str());

            double fac = 10000; // for comparison and rounding
            double result;

            vector<interp> usHead;
            usHead.push_back(interp(0, 0.1, 1));
            usHead.push_back(interp(0, 0.595, 1));
            usHead.push_back(interp(0, 1.09, 1.09));
            usHead.push_back(interp(0, 969.652500, 969.652500));
            usHead.push_back(interp(1.93, 3.220909, 3.221075));
            usHead.push_back(interp(12446.50, 10.247500, 387.335538));
            usHead.push_back(interp(563.23, 7.084347, 8.169765));
            usHead.push_back(interp(300, 6, 6.41130084851644));
            usHead.push_back(interp(300, 9.8, 9.96662156471763));
            usHead.push_back(interp(600, 9.8, 10.5545663378537));
            usHead.push_back(interp(700, 9.8, 10.9814157420724));
            usHead.push_back(interp(1200, 9.75, 13.5181649665682));
            usHead.push_back(interp(2000, 0, 21.0069129227487));
            usHead.push_back(interp(2000, 400, 411.821875347824));

            for (auto p: usHead)
            {
                if (hpg.InterpUpstreamHead(p.flow, p.in, result))
                {
                    Assert::Fail((wstring(L"InterpUpstreamHead failed for Q=") + to_wstring(p.flow) + wstring(L" yUp=") + to_wstring(p.in)).c_str());
                }
                p.out = result;
                Assert::AreEqual(round(p.expected*fac), round(result*fac));
            }
        }


        TEST_METHOD(BsplineTest)
        {
            using namespace std;
            double* x = new double[80];
            double* y = new double[80];

            for (int i = 0; i < 80; i++)
            {
                x[i] = data[i * 4 + 0];
                y[i] = data[i * 4 + 1];
            }

            //BSpline<double> spline(x, 80, y, 0, BSpline<double>::BC_ZERO_SECOND);
        }
        
        
		TEST_METHOD(LoadHpgTest)
		{
            using namespace std;
            bool status;
            
            hpgInit();

            hpg::Hpg hpg;
            Assert::IsTrue(hpg.LoadFromFile(hpgPath), makeInfo(L"Failed to load HPG: ", hpg.getErrorMessage()).c_str());
            Assert::AreEqual(hpg.getMaxDepthFraction(), 1., L"Failed to read header properly");
        }
        
		TEST_METHOD(FindFlowIncrementsTest)
		{
            using namespace std;
            bool status;

            xs::Reach reach = makeReach(1, 500);
            
            double volume = 0;
            double hf = 0;
            int error = 0;
            double yUp = 0;

            int nC = 500*0.3048;
            double fac = 100.;
            double maxDepthFrac = 1.;

            //if (ComputeCombinedProfile(reach, 24189.25, 9.505, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            if (ComputeCombinedProfile(reach, 0, 0.1, nC, false, false, g, kn, maxDepthFrac, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for full insanity");
            }
            
            //int nC = 500*0.3048;
            //double volume = 0;
            //double hf = 0;
            //int error = 0;
            //double yUp = 0;
            //double maxDepthFrac = 1.;
            //ComputeFreeProfile(reach, 5000, 9.998, nC, false, true, g, kn, maxDepthFrac, yUp, volume, hf);

            //std::deque<double> flows;
            //HpgCreator c;
            //c.setNumBackwaterSteps((int)round(reach.getLength() / 10.0));

            //double maxDepth1, maxDepth2;
            //double maxPosFlow = c.findMaxFlow(reach, false, maxDepth1);
            //double maxAdvFlow = c.findMaxFlow(reach, true, maxDepth2);

            //c.findFlowIncrements(reach, false, 0, maxDepth1, flows);


            //double pressurizedHeight = 300;
            //double minFlow = maxPosFlow * 1.02;
            //auto x = reach.getXs();
            //double headDiff = (pressurizedHeight - reach.getDsInvert());
            //double maxFlow = x->computeArea(x->getMaxDepth()) * 2. * std::sqrt(2. * g * headDiff * 3.); // I pulled the *3 out of nowhere hahaha
            //for (int i = 0; i < 20; i++)
            //{
            //    flows.push_back(minFlow + (maxFlow - minFlow) * (double)(i) / 20.);
            //}

            ////std::vector<double> crit1;
            ////std::vector<double> crit2;

            ////for (int i = 0; i < 20; i++)
            ////{
            ////    double y = reach.getMaxDepth() * (double)(i+1) / 20.;
            ////    double q;
            ////    ComputeCriticalFlow(reach, y, g, q);
            ////    crit1.push_back(y);
            ////    crit2.push_back(q);
            ////}

            ////std::vector<double> critCurve;

            ////for (auto q: flows)
            ////{
            ////    double yc;
            ////    ComputeCriticalDepth(reach, q, g, yc);
            ////    critCurve.push_back(yc);
            ////}

            to_string(1);
        }

        template<class T>
        bool vectorEqual(const std::vector<T>& v1, const std::vector<T>& v2) const
        {
            if (v1.size() != v2.size())
            {
                return false;
            }

            for (int i = 0; i < v1.size(); i++)
            {
                if (v1[i] != v2[i])
                    return false;
            }

            return true;
        }

        std::wstring makeInfo(wchar_t* p1, std::string p2)
        {
            std::wstring str = p1;
            str = str + std::wstring(p2.begin(), p2.end());
            return str;
        }

    private:
        double* data;
	};

}
//
//namespace Microsoft{
//    namespace VisualStudio {
//        namespace CppUnitTestFramework {
//
//            template<>
//            static std::wstring ToString<DateTime>(const DateTime& dt) {
//                return dt.toUnicodeString();
//            }
//
//        }
//    }
//}
