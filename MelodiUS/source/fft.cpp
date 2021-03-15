/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "fft.h"
#include "globaldef.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>

#include "benchmark.h"


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr int MAX_DEPTH = 4;


/*****************************************************************************/
/* Classes ----------------------------------------------------------------- */
class OddVector
{
public:
    constexpr static int even = 0;
    constexpr static int odd  = 1;

private:
    std::vector<complex_t>& m_originalVector;
    int                     m_depth     = 1;
    int                     m_oddOrEven = even;

public:
    OddVector() = delete;
    OddVector(OddVector& other, int oddOrEven)
    : m_originalVector{other.m_originalVector}, m_oddOrEven{oddOrEven}, m_depth{other.m_depth + 1}
    {
        assert(oddOrEven == odd || oddOrEven == even);
    }
    OddVector(std::vector<complex_t>& otherVec, int oddOrEven)
    : m_originalVector{otherVec}, m_oddOrEven{oddOrEven}
    {
        assert(oddOrEven == odd || oddOrEven == even);
    }

    [[nodiscard]] complex_t& operator[](size_t index)
    {
        return m_originalVector[index * 2 * m_depth + m_oddOrEven];
    }

    [[nodiscard]] std::vector<complex_t>::iterator begin()
    {
        return m_originalVector.begin();
    }
    [[nodiscard]] std::vector<complex_t>::iterator end()
    {
        return m_originalVector.end();
    }

    [[nodiscard]] size_t size()
    {
        return m_originalVector.size() % (2 * m_depth) == 0
                 ? m_originalVector.size() / (2 * m_depth)
                 : m_originalVector.size() / (2 * m_depth) + 1;
    }
};


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
double FindFrequency(const Recording& audio)
{
    Benchmark total{"Total"};
    // To take only a single channel
    /* clang-format off */
    auto lmbd = [&, N = audio.getNumChannels()](const SAMPLE& c) mutable
                {
                    (void)c;
                    return static_cast<bool>(--N == 0 ? N = audio.getNumChannels() : false);
                };
    /* clang-format on */

    /*std::vector<complex_t> v(audio.getNumSamples() / audio.getNumChannels());
    std::copy_if(audio.begin(), audio.end(), v.begin(), lmbd);*/
    std::vector<complex_t> v(audio.begin(), audio.end());

    // Calculate FFT
    FFT(v);


    /* clang-format off */
    size_t peak = std::distance(v.begin(),
                                std::max_element(v.begin() + 1,
                                                 v.end() - v.size() / 2,
                                                 [](const complex_t& c1, const complex_t& c2)
                                                 {
                                                     return std::abs(c1) < std::abs(c2);
                                                 }));
    /* clang-format on */
    double freq = peak / audio.getNumSeconds();
    return freq;
}

void FFT_op(OddVector& x, int depth)
{
    if(x.size() <= 1)
    {
        return;
    }

    // divide
    Benchmark* copy = nullptr;
    OddVector  even(x, OddVector::even);
    OddVector  odd(x, OddVector::odd);
    delete copy;

    // conquer
    if(depth > MAX_DEPTH)
    {
        FFT_op(even, depth + 1);
        FFT_op(odd, depth + 1);
    }
    else
    {
        std::thread evenThread{FFT_op, std::ref(even), depth + 1};
        // std::thread oddThread{FFT, std::ref(odd), depth};
        FFT_op(odd, depth + 1);

        evenThread.join();
        // oddThread.join();
    }

    // combine
    Benchmark* combine = nullptr;
    if(depth < MAX_DEPTH)
    {
        combine = new Benchmark{"combine"};
    }
#pragma omp parallel for
    for(size_t k = 0; k < x.size() / 2; ++k)
    {
        complex_t t         = std::polar(1.0, -2 * pi * k / x.size()) * odd[k];
        x[k]                = even[k] + t;
        x[k + x.size() / 2] = even[k] - t;
    }
    delete combine;
}

// https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
void FFT(std::vector<complex_t>& x, int depth)
{
    if(x.size() <= 1)
    {
        return;
    }

    // divide
    Benchmark* copy = nullptr;
    OddVector  even(x, OddVector::even);
    OddVector  odd(x, OddVector::odd);
    delete copy;

    // conquer
    if(depth > MAX_DEPTH)
    {
        FFT_op(even, depth + 1);
        FFT_op(odd, depth + 1);
    }
    else
    {
        std::thread evenThread{FFT_op, std::ref(even), depth + 1};
        // std::thread oddThread{FFT, std::ref(odd), depth};
        FFT_op(odd, depth + 1);

        evenThread.join();
        // oddThread.join();
    }

    // combine
    Benchmark* combine = nullptr;
    if(depth < MAX_DEPTH)
    {
        combine = new Benchmark{"combine"};
    }
#pragma omp parallel for
    for(size_t k = 0; k < x.size() / 2; ++k)
    {
        complex_t t         = std::polar(1.0, -2 * pi * k / x.size()) * odd[k];
        x[k]                = even[k] + t;
        x[k + x.size() / 2] = even[k] - t;
    }
    delete combine;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
