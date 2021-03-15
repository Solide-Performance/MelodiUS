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

// https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
void FFT(std::vector<complex_t>& x, int depth)
{
    Benchmark* fft = nullptr;
    if(depth < MAX_DEPTH)
    {
        fft = new Benchmark{"FFT"};
    }
    if(x.size() <= 1)
    {
        return;
    }

    // divide
    const size_t           elements = x.size() % 2 == 0 ? x.size() / 2 : x.size() / 2 + 1;
    std::vector<complex_t> even(elements);
    std::vector<complex_t> odd(elements);

    /* clang-format off */
    // For vector slicing in even and odd pairs
    auto lmbd = [N = false](const complex_t& c) mutable
                {
                    (void)c;
                    return N = !N;
                };
    /* clang-format on */

    Benchmark* copy = nullptr;
    if(depth < MAX_DEPTH)
    {
        copy = new Benchmark{"copy"};
    }
    std::copy_if(x.begin(), x.end(), even.begin(), lmbd);
    std::copy_if(x.begin() + 1, x.end(), odd.begin(), lmbd);
    delete copy;

    // conquer
    if(depth > MAX_DEPTH)
    {
        FFT(even, depth + 1);
        FFT(odd, depth + 1);
    }
    else
    {
        std::thread evenThread{FFT, std::ref(even), depth + 1};
        // std::thread oddThread{FFT, std::ref(odd), depth};
        FFT(odd, depth + 1);

        evenThread.join();
        // oddThread.join();
    }

// combine
#pragma omp parallel for
    Benchmark* combine = nullptr;
    if(depth < MAX_DEPTH)
    {
        combine = new Benchmark{"combine"};
    }
    for(size_t k = 0; k < x.size() / 2; ++k)
    {
        complex_t t         = std::polar(1.0, -2 * pi * k / x.size()) * odd[k];
        x[k]                = even[k] + t;
        x[k + x.size() / 2] = even[k] - t;
    }
    delete combine;

    delete fft;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
