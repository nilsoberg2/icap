#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>

#include "../../icap/icap.h"
#include "../../util/math.h"


namespace fs = boost::filesystem;

void main(int argc, char** argv)
{
    using namespace std;
    bool status;

    if (argc < 2)
    {
        cout << "Require two command line arguments" << endl;
        return;
    }

    string inputFile = argv[1];
    string reportFile = inputFile.substr(0, inputFile.length() - 3) + "rpt";
    string outputFile = inputFile.substr(0, inputFile.length() - 3) + "out";
    
    ICAP icap;
    
    if (!icap.Open(inputFile, reportFile, outputFile, true))
    {
        cout << "Unable to open the ICAP file: " << icap.getErrorMessage() << endl;
        return;
    }

    if (!icap.Start())
    {
        cout << "Unable to start ICAP: " << icap.getErrorMessage() << endl;
        return;
    }

    double curStep = 0.0;
    double totalDuration = icap.GetTotalDuration();

    cout << "Iterating...\n" << endl;
    // Now iterate over each timestep and route the system for that step.
    bool toContinue = true;
    int iterCount = 0;
    do
    {
        cout << "Iteration " << curStep << " of " << totalDuration << "                  \r" << flush;
        if (!icap.Step(&curStep, false))
        {
            cout << "Error in step: " << icap.getErrorMessage() << endl;
            break;
        }
    } while (curStep <= totalDuration && !isZero(curStep));
    cout << endl;

    //string inputFile = "..\\..\\icap-mex\\lawrence_july.inp";
    //string reportFile = "..\\..\\icap-mex\\report.txt";
    //string outputFile = "..\\..\\icap-mex\\output.out";

    //ICAP icap;
    //icap.EnableRealTimeStatus();
    //if (!icap.Open(inputFile, reportFile, outputFile, true))
    //{
    //    //Assert::Fail(makeInfo(L"Failed to open icap: ", icap.getErrorMessage()).c_str());
    //}
    //
    //if (!icap.Start(false))
    //{
    //    //Assert::Fail(makeInfo(L"Failed to start icap: ", icap.getErrorMessage()).c_str());
    //}
    //
    //icap.AddSource("Melvina");

    //for (int i = 0; i < 9; i++)
    //{
    //    icap.InitializeZeroFlows();
    //    icap.SetCurrentNodeInflow("Melvina", i * 100);
    //    icap.SetCurrentNodeHead("Outlet", 380.0);

    //    double tm;
    //    auto retval = icap.Step(&tm, 1, false);

    //    std::string error = icap.getErrorMessage();
    //    //Assert::AreEqual(true, retval, std::wstring(error.begin(), error.end()).c_str());

    //    double val = icap.GetCurrentNodeInflow("Outlet");
    //    //Assert::AreEqual(val, i * 100.0);

    //    val = icap.GetCurrentNodeHead("Outlet");
    //    //Assert::AreEqual(val, 380.0);

    //    if (i == 0)
    //    {
    //        val = icap.GetCurrentNodeHead("Kildare");
    //        //Assert::AreEqual(380.0, val, L"Expected Kildare to be equal to Outlet for zero flow");
    //    }
    //    else
    //    {
    //        val = icap.GetCurrentNodeHead("Kildare");
    //        //Assert::AreNotEqual(380.0, val, L"Expected Kildare to be not equal to Outlet for zero flow");
    //    }
    //}
}
