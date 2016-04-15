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


#ifdef BUILDEXE

#include "hpg_creator.hpp"
#include <iostream>
#include <hpg/units.hpp>
#include <string>


void PrintUsage(char* progName);
int Interactive();


int main(int argc, char** argv)
{
    if (argc >= 7)
    {
        double diameter = atof(argv[1]);
        double length = atof(argv[2]);
        double roughness = atof(argv[3]);
        double slope = atof(argv[4]);
        double dsInvert = atof(argv[5]);
        //output file is argv[6]
        int nodeID = -1;
        if (argc >= 8)
            nodeID = atoi(argv[7]);
        double dsStation = 0.0;
        if (argc >= 9)
            dsStation = atof(argv[8]);
        double unsteadyDepth = 0.80;
        if (argc >= 10)
            unsteadyDepth = atof(argv[9]);

        long result = hpgAutoCreateHPG(diameter, length, roughness, slope, dsInvert, unsteadyDepth, hpg::units::english, argv[6], nodeID, dsStation);
		getchar();
        if (result)
        {
            std::cout << "An error occurred (error# " << result << ").  HPG was not created." << std::endl;
            return 1;
        }
    }
    else if (argc == 2 && !strcmp(argv[1], "-h"))
    {
        PrintUsage(argv[0]);
        return 1;
    }
    else
    {
        return Interactive();
    }
}


void PrintUsage(char* progName)
{
    // small_inc_range defaults to 0.20 and is ignored.
    // units default to english
    std::cout << "USAGE: " << progName << " <diameter> <length> <roughness> <slope> <dsInvert> \n"
        << "    <output_file> <nodeID> <dsStation> <this->maxDepthFrac>\n\n"
        << "where <this->maxDepthFrac> defaults to 0.80 (pipe full = 80% of diameter).\n"
        << "<nodeID> and <dsStation> are optional." << std::endl;
}


int Interactive()
{
    using namespace std;

    double diameter;
    cout << "Enter pipe diameter                   : ";
    cin >> diameter;

    double length;
    cout << "Enter pipe length                     : ";
    cin >> length;
    
    double roughness;
    cout << "Enter pipe roughness                  : ";
    cin >> roughness;
    
    double slope;
    cout << "Enter pipe slope                      : ";
    cin >> slope;
    
    double dsInvert;
    cout << "Enter pipe downstream invert          : ";
    cin >> dsInvert;

    string file;
    cout << "Enter HPG file name                   : ";
    cin >> file;

    int nodeID;
    cout << "Enter HPG node ID                     : ";
    cin >> nodeID;

    double dsStation;
    cout << "Enter pipe downstream station         : ";
    cin >> dsStation;

    double unsteadyDepth;
    cout << "Enter unsteady depth (hint: 0.80)     : ";
    cin >> unsteadyDepth;

    long result = hpgAutoCreateHPG(diameter, length, roughness, slope, dsInvert, unsteadyDepth, hpg::units::english, (char*)file.c_str(), nodeID, dsStation);
    if (result)
    {
        std::cout << "An error occurred (error# " << result << ").  HPG was not created." << std::endl;
        return 1;
    }

    return 0;
}

#endif
