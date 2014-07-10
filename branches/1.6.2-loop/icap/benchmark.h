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
