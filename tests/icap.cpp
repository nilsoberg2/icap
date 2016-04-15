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

#include "stdafx.h"
#include "CppUnitTest.h"
#include <Windows.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/log/core.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>

#include "../icap/icap.h"
#include "../util/math.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = boost::filesystem;

const double kn = 1.485918577496261;
const double g = 32.1740;// 32.185039370078741;

namespace TestGeometryRead
{		
	TEST_CLASS(IcapTest)
	{
	public:
        
		TEST_METHOD(TestIcap)
		{
            using namespace std;
            bool status;

            //string inputFile = "..\\test\\geometry_test.inp";
            //string reportFile = "..\\test\\report.txt";
            //string outputFile = "..\\test\\output.out";
            string inputFile = "..\\geometry_test.inp";
            string reportFile = "..\\test\\report.txt";
            string outputFile = "..\\test\\output.out";
            //This is a test.
            
            boost::log::add_file_log("testing.log");

            ICAP icap;
            icap.EnableRealTimeStatus();

            if (!icap.Open(inputFile, reportFile, outputFile, true))
            {
                Assert::Fail(makeInfo(L"Failed to open icap: ", icap.getErrorMessage()).c_str());
            }
            
            if (!icap.Start(false))
            {
                Assert::Fail(makeInfo(L"Failed to start icap: ", icap.getErrorMessage()).c_str());
            }

            icap.AddSource("Melvina");

            for (int i = 0; i < 9; i++)
            {
                icap.InitializeZeroFlows();
                icap.SetCurrentNodeInflow("Melvina", i * 100);
                icap.SetCurrentNodeHead("Outlet", 380.0);

                double tm;
                auto retval = icap.Step(&tm, 1, false);

                std::string error = icap.getErrorMessage();
                Assert::AreEqual(true, retval, std::wstring(error.begin(), error.end()).c_str());

                double val = icap.GetCurrentNodeInflow("Outlet");
                Assert::AreEqual(val, i * 100.0);

                val = icap.GetCurrentNodeHead("Outlet");
                Assert::AreEqual(val, 380.0);

                if (i == 0)
                {
                    val = icap.GetCurrentNodeHead("Kildare");
                    Assert::AreEqual(380.0, val, L"Expected Kildare to be equal to Outlet for zero flow");
                }
                else
                {
                    val = icap.GetCurrentNodeHead("Kildare");
                    Assert::AreNotEqual(380.0, val, L"Expected Kildare to be not equal to Outlet for zero flow");
                }
            }
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
