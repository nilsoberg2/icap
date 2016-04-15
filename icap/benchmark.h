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

#ifndef __BENCHMARK_H_______________________20080509112626__
#define __BENCHMARK_H_______________________20080509112626__


// BENCHMARKYES should be defined in a special project configuration if
// benchmarking is to be enabled.
#if defined(BENCHMARKYES)

#include <iostream>
#include <fstream>
#include <windows.h>

typedef struct {
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
} stopWatch;

class CStopWatch {

private:
    stopWatch timer;
    LARGE_INTEGER frequency;
    double LIToSecs( LARGE_INTEGER & L) ;
public:
    CStopWatch() ;
    void startTimer( ) ;
    void stopTimer( ) ;
    double getElapsedTime() ;
};


#define BENCH_INIT          CStopWatch _sw;
#define BENCH_START         _sw.startTimer();
#define BENCH_STOP          _sw.stopTimer();
#define BENCH_REPORT        _sw.getElapsedTime() // has no ; because it might be embedded
#define BENCH_REPORT_IOS(X) m_benchFile << X << "Elapsed time: " << BENCH_REPORT << std::endl;


class BenchmarkVariables
{
public:
    // Benchmark
    double m_hpgLoadingTime;
    double m_hpgGetUpstreamTime;
    double m_hpgGetVolumeTime;
    double m_hpgCount;
    std::ofstream m_benchFile;

    BenchmarkVariables();
    ~BenchmarkVariables();
};

#else


#define BENCH_INIT          
#define BENCH_START         
#define BENCH_STOP          
#define BENCH_REPORT        
#define BENCH_REPORT_IOS(X) 


#endif


#endif//__BENCHMARK_H_______________________20080509112626__
