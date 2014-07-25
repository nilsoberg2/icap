#include "stdafx.h"
#include "CppUnitTest.h"
#include <Windows.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>

#include "../hpg_creation/profile.h"
#include "../hpg_creation/normcrit.h"
#include "../xslib/circular.h"
#include "../util/math.h"
#define NORMCRIT_STATIC
#include "../../../../hpg/oberg/cpp/normcrit/trunk/normcrit.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = boost::filesystem;

const double kn = 1.485918577496261;
const double g = 32.1740;// 32.185039370078741;

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

            if (ComputeProfile(reach, 500, 10, nC, false, g, kn, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for full 500,10");
            }
            Assert::AreEqual(round(yUp*fac), round(9.524223323566060*fac), makeInfo(L"Failed full-ds test: yUp should be 9.524223323566060 but is ", std::to_string(yUp)).c_str());

            if (ComputeProfile(reach, 1000., 0, nC, false, g, kn, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for empty fac,0");
            }
            Assert::AreEqual(round(yUp*fac), round(10.083769115971373*fac), makeInfo(L"Failed ds-empty test: yUp should be 10.083769115971373 but is ", std::to_string(yUp)).c_str());

            if (ComputeProfile(reach, 500, 8, nC, false, g, kn, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for 500,5");
            }
            Assert::AreEqual(round(7.578609087468569*fac), round(yUp*fac), makeInfo(L"Failed open test: yUp should be 7.578609087468569 but is ", std::to_string(yUp)).c_str());

            if (ComputeProfile(reach, 1000., 12, nC, false, g, kn, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for fac,12");
            }
            Assert::AreEqual(round(13.434423078981334*fac), round(yUp*fac), makeInfo(L"Failed full pressurized test: yUp should be 13.434423078981334 but is ", std::to_string(yUp)).c_str());

            if (ComputeProfile(reach, 0, 8, nC, false, g, kn, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for 0,8");
            }
            Assert::AreEqual(round(7*fac), round(yUp*fac), makeInfo(L"Failed zero test: yUp should be 7 but is ", std::to_string(yUp)).c_str());

            if (ComputeProfile(reach, 0, 12, nC, false, g, kn, yUp, volume, hf))
            {
                Assert::Fail(L"Failed to converge to solution for 0,12");
            }
            Assert::AreEqual(round(11*fac), round(yUp*fac), makeInfo(L"Failed zero+full test: yUp should be 11 but is ", std::to_string(yUp)).c_str());
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
