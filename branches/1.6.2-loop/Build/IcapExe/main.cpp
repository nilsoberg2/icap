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

    string inputFile = "..\\test\\geometry_test.inp";
    string reportFile = "..\\test\\report.txt";
    string outputFile = "..\\test\\output.out";

    ICAP icap;
    icap.SetRealTimeStatus(true);
    if (!icap.Open(inputFile, reportFile, outputFile, true))
    {
        //Assert::Fail(makeInfo(L"Failed to open icap: ", icap.getErrorMessage()).c_str());
    }
    
    if (!icap.Start(false))
    {
        //Assert::Fail(makeInfo(L"Failed to start icap: ", icap.getErrorMessage()).c_str());
    }
}
