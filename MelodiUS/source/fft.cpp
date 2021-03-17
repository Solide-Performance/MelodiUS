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

    std::copy_if(x.begin(), x.end(), even.begin(), lmbd);
    std::copy_if(x.begin() + 1, x.end(), odd.begin(), lmbd);

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


    // for(size_t k = 0; k < x.size() / 2; ++k)
    //{
    //    complex_t* begin_odd  = &odd[k];
    //    complex_t* begin_even = &even[k];

    //    double re_odd[2]  = {begin_odd[0].real(), begin_odd[1].real()};
    //    double im_odd[2]  = {begin_odd[0].imag(), begin_odd[1].imag()};
    //    double re_even[2] = {begin_even[0].real(), begin_even[1].real()};
    //    double im_even[2] = {begin_even[0].imag(), begin_even[1].imag()};

    //    double re_t   = std::cos(-2 * pi * k / x.size());
    //    double im_t   = std::sin(-2 * pi * k / x.size());
    //    double re_mul = re_odd[0];
    //    double im_mul = im_odd[0];

    //    /* Complex number multiplication */
    //    double tmp = re_t * re_mul - im_t * im_mul;
    //    im_t       = re_t * im_mul + im_t * re_mul;
    //    re_t       = tmp;

    //    double im_new_p = im_even[0] + im_t;
    //    double re_new_p = re_even[0] + re_t;
    //    double im_new_n = im_even[0] - im_t;
    //    double re_new_n = re_even[0] - re_t;

    //    x[k]                = {re_new_p, im_new_p};
    //    x[k + x.size() / 2] = {re_new_n, im_new_n};
    //}

#pragma omp parallel for
    // for(size_t k = 0; k < x.size() / 2; ++k)
    //{
    //    double re_t   = std::cos(-2 * pi * k / x.size());
    //    double im_t   = std::sin(-2 * pi * k / x.size());
    //    double re_mul = odd[k].real();
    //    double im_mul = odd[k].imag();

    //    /* Complex number multiplication */
    //    double tmp = re_t * re_mul - im_t * im_mul;
    //    im_t       = re_t * im_mul + im_t * re_mul;
    //    re_t       = tmp;

    //    complex_t t = {re_t, im_t};

    //    double im_new_p = even[k].imag() + im_t;
    //    double re_new_p = even[k].real() + re_t;
    //    double im_new_n = even[k].imag() - im_t;
    //    double re_new_n = even[k].real() - re_t;

    //    x[k]                = {re_new_p, im_new_p};
    //    x[k + x.size() / 2] = {re_new_n, im_new_n};
    //}

#pragma omp parallel for
    // for(size_t k = 0; k < x.size() / 2; ++k)
    //{
    //    complex_t t         = std::polar<complex_t::_Ty>(1.0, -2 * pi * k / x.size()) * odd[k];
    //    x[k]                = even[k] + t;
    //    x[k + x.size() / 2] = even[k] - t;
    //}
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
