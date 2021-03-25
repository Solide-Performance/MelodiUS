/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "fft.h"
#include "globaldef.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>

#include "benchmark.h"

#ifndef LINUX_
#include "fasttrigo/fasttrigo.h"
#include <immintrin.h>
#include <xmmintrin.h>
#endif


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr int    MAX_DEPTH       = 4;
constexpr double MIN_GUITAR_FREQ = 75.0;


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
size_t FindPeak(const std::vector<complex_t>& v,
                std::vector<size_t>           ignoredPeaks = {},
                size_t                        trueBegin    = 1)
{
    /* clang-format off */
    return std::distance(v.begin(),
                         std::max_element(v.begin() + trueBegin,
                                          v.end() - v.size() / 2,
                                          [v, ignoredPeaks](const complex_t& c1, const complex_t& c2)
                                          {
                                              for (size_t ignoredPeak : ignoredPeaks)
                                              {
                                                  if(c1 == v[ignoredPeak] || c2 == v[ignoredPeak])
                                                  {
                                                      return false;
                                                  }
                                              }
                                              return std::abs(c1) < std::abs(c2);
                                          }));
    /* clang-format on */
}

size_t ComparePeaks(const std::vector<complex_t>& v, size_t peak1, size_t peak2)
{
    constexpr int amplitudeMargin = 200;
    float         num             = abs(abs(v[peak1]) - abs(v[peak2]));
    if(num < amplitudeMargin)
    {
        return std::min(peak1, peak2);
    }
}


double FindFrequency(const Recording& audio)
{
    // Benchmark total{"Total"};
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


    size_t peak  = FindPeak(v);
   /* size_t peak2 = FindPeak(v, {peak});

    peak = ComparePeaks(v, peak, peak2);*/


    double freq = peak / audio.getNumSeconds();
    /*std::vector<size_t> badPeaks{};
    while(freq < MIN_GUITAR_FREQ)
    {
        badPeaks.push_back(peak);

        if (badPeaks.size() > 10)
        {
            return -1.;
        }

        size_t newPeak = FindPeak(v, badPeaks, *std::max_element(badPeaks.begin(), badPeaks.end()));
        peak           = ComparePeaks(v, peak2, newPeak);

        double freq = peak / audio.getNumSeconds();
    }*/

    return freq;
}

// https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
void FFT(std::vector<complex_t>& x, int depth)
{
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

    // Copy data in odd and even sample pairs
    std::copy_if(x.cbegin(), x.cend(), even.begin(), lmbd);
    std::copy_if(x.cbegin() + 1, x.cend(), odd.begin(), lmbd);

    // conquer
    if(depth > MAX_DEPTH)
    {
        FFT(even, depth + 1);
        FFT(odd, depth + 1);
    }
    else
    {
        std::thread evenThread{FFT, std::ref(even), depth + 1};
        FFT(odd, depth + 1);

        evenThread.join();
    }

// combine
#ifndef LINUX_
    size_t N = x.size() / 2;
#pragma omp parallel for
    for(size_t k = 0; k < (N & ~(4 - 1)); k += 4)
    {
        complex_t* begin_odd  = &odd[k];
        complex_t* begin_even = &even[k];

        /* clang-format off */
        __m128 mm_re_odd  = {begin_odd[0].real(), begin_odd[1].real(),
                             begin_odd[2].real(), begin_odd[3].real()};
        __m128 mm_im_odd  = {begin_odd[0].imag(), begin_odd[1].imag(),
                             begin_odd[2].imag(), begin_odd[3].imag()};
        __m128 mm_re_even = {begin_even[0].real(), begin_even[1].real(),
                             begin_even[2].real(), begin_even[3].real()};
        __m128 mm_im_even = {begin_even[0].imag(), begin_even[1].imag(),
                             begin_even[2].imag(), begin_even[3].imag()};
        /* clang-format on */

        __m128           mm_theta     = {k, k + 1, k + 2, k + 3};
        constexpr __m128 mm_thetamult = {-2.f * pi, -2.f * pi, -2.f * pi, -2.f * pi};

        // https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_set_ps1
        __m128 mm_xs = _mm_set_ps1(x.size());

        // https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_div_ps
        // https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_mul_ps
        mm_theta = _mm_mul_ps(_mm_div_ps(mm_theta, mm_xs), mm_thetamult);


        __m128 mm_re_t = FT::cos_ps(mm_theta);
        __m128 mm_im_t = FT::sin_ps(mm_theta);

        /* Complex number multiplication */
        // https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_sub_ps
        // https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_add_ps
        __m128 mm_tmp = _mm_sub_ps(_mm_mul_ps(mm_re_t, mm_re_odd), _mm_mul_ps(mm_im_t, mm_im_odd));
        mm_im_t       = _mm_add_ps(_mm_mul_ps(mm_re_t, mm_im_odd), _mm_mul_ps(mm_im_t, mm_re_odd));
        mm_re_t       = mm_tmp;

        __m128 mm_im_p = _mm_add_ps(mm_im_even, mm_im_t);
        __m128 mm_re_p = _mm_add_ps(mm_re_even, mm_re_t);
        __m128 mm_im_n = _mm_sub_ps(mm_im_even, mm_im_t);
        __m128 mm_re_n = _mm_sub_ps(mm_re_even, mm_re_t);

        for(size_t i = 0; i < 4; i++)
        {
            // https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_extract_ps
            float re_p = mm_re_p.m128_f32[i];
            float re_n = mm_re_n.m128_f32[i];
            float im_p = mm_im_p.m128_f32[i];
            float im_n = mm_im_n.m128_f32[i];

            x[k + i]                = {re_p, im_p};
            x[k + i + x.size() / 2] = {re_n, im_n};
        }
    }

#else
#pragma omp parallel for
    for(size_t k = 0; k < x.size() / 2; ++k)
    {
        complex_t t         = std::polar<complex_t::_Ty>(1.0, -2 * pi * k / x.size()) * odd[k];
        x[k]                = even[k] + t;
        x[k + x.size() / 2] = even[k] - t;
    }
#endif
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
