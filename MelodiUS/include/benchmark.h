#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "globaldef.h"
#include <chrono>
#include <iostream>


/*****************************************************************************/
/* Defines ----------------------------------------------------------------- */
#define BENCHMARK_CODE true


/*****************************************************************************/
/* Classes ----------------------------------------------------------------- */
class Benchmark
{
private:
    using clock     = std::chrono::high_resolution_clock;
    using timepoint = std::chrono::time_point<clock>;
    using duration  = std::chrono::duration<uint64_t, std::milli>;

    timepoint   m_startTime;
    const char* m_msg;

public:
    Benchmark(const char* msg = "") : m_msg{msg}, m_startTime{clock::now()}
    {
    }
    ~Benchmark()
    {
        using namespace std::chrono;

        timepoint endTime = clock::now();
        duration  dur     = duration_cast<duration>(endTime - m_startTime);

        std::cout << m_msg << " Took: " << dur.count() << "ms" << std::endl;
    }
};


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
