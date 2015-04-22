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
#define NORMCRIT_STATIC
#include "../../../../hpg/oberg/cpp/normcrit/trunk/normcrit.h"


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


        static bool idComp(const std::string& id1, const std::string& id2)
        {
            using namespace std;
            cout << id1 << " " << id2 << endl;
            return id1 == id2;
        }

        xs::Reach makeReach(double usInvert, double length)
        {
            xs::Reach reach;
            reach.setLength(length);
            reach.setRoughness(0.015);
            reach.setDsInvert(0);
            reach.setUsInvert(usInvert);
            reach.setXs(std::shared_ptr<xs::CrossSection>(new xs::Circular(10)));
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

            testReach1();
            testReach2();
            testReach3();
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
