
#if defined(BENCHMARKYES)


#include <windows.h>
#include "benchmark.h"


double CStopWatch::LIToSecs( LARGE_INTEGER & L) {
    return ((double)L.QuadPart /(double)frequency.QuadPart) ;
}

CStopWatch::CStopWatch(){
    timer.start.QuadPart=0;
    timer.stop.QuadPart=0;
    QueryPerformanceFrequency( &frequency ) ;
}

void CStopWatch::startTimer( ) {
    QueryPerformanceCounter(&timer.start) ;
}

void CStopWatch::stopTimer( ) {
    QueryPerformanceCounter(&timer.stop) ;
}

double CStopWatch::getElapsedTime() {
    LARGE_INTEGER time;
    time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
    return LIToSecs( time) ;
}



BenchmarkVariables::BenchmarkVariables()
{
    m_hpgLoadingTime = 0.0;
    m_hpgGetUpstreamTime = 0.0;
    m_hpgGetVolumeTime = 0.0;
    m_hpgCount = 0.0;
    m_benchFile.open(L"c:\\temp\\icap_benchmark.txt");
    if (! m_benchFile.is_open())
        m_benchFile.open(L"icap_benchmark.txt");
}

BenchmarkVariables::~BenchmarkVariables()
{
    m_benchFile.close();
}


#endif
