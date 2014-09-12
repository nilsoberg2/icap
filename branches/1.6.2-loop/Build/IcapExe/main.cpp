#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>

#include "../../icap/icap.h"
#include "../../util/math.h"


namespace fs = boost::filesystem;

void main()
{
    using namespace std;
    bool status;
    
    string inputFile = "..\\..\\icap-mex\\lawrence_july.inp";
    string reportFile = "..\\..\\icap-mex\\report.txt";
    string outputFile = "..\\..\\icap-mex\\output.out";

    ICAP icap;
    icap.EnableRealTimeStatus();
    if (!icap.Open(inputFile, reportFile, outputFile, true))
    {
        //Assert::Fail(makeInfo(L"Failed to open icap: ", icap.getErrorMessage()).c_str());
    }
    
    if (!icap.Start(false))
    {
        //Assert::Fail(makeInfo(L"Failed to start icap: ", icap.getErrorMessage()).c_str());
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
        //Assert::AreEqual(true, retval, std::wstring(error.begin(), error.end()).c_str());

        double val = icap.GetCurrentNodeInflow("Outlet");
        //Assert::AreEqual(val, i * 100.0);

        val = icap.GetCurrentNodeHead("Outlet");
        //Assert::AreEqual(val, 380.0);

        if (i == 0)
        {
            val = icap.GetCurrentNodeHead("Kildare");
            //Assert::AreEqual(380.0, val, L"Expected Kildare to be equal to Outlet for zero flow");
        }
        else
        {
            val = icap.GetCurrentNodeHead("Kildare");
            //Assert::AreNotEqual(380.0, val, L"Expected Kildare to be not equal to Outlet for zero flow");
        }
    }
}
