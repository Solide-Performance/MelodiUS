/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "fft.h"
#include "globaldef.h"
#include <algorithm>
#include <cmath>
#include <numbers>


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
double FindFrequency(const Recording& audio)
{
    // To take only a single channel
    /* clang-format off */
    auto lmbd = [&, N = audio.getNumChannels()](const SAMPLE& c) mutable
                {
                    return --N == 0 ? N = audio.getNumChannels() : false;
                };
    /* clang-format on */

    //std::vector<complex_t> v(audio.getNumSamples() / audio.getNumChannels());
    //std::copy_if(audio.begin(), audio.end(), v.begin(), lmbd);
    std::vector<complex_t> v(audio.begin(), audio.end());

    // Calculate FFT
    FFT(v);

    /* clang-format off */
    double firstpeak = std::distance(v.begin(),
                                std::max_element(v.begin() + 1,
                                                 v.end(),
                                                 [](const complex_t& c1, const complex_t& c2)
                                                 {
                                                     return std::abs(c1) < std::abs(c2);
                                                 }));
    /* clang-format off */

    double freq = firstpeak / audio.getNumSeconds();
    return freq;
}

// https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
void FFT(std::vector<complex_t>& x)
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
    auto lmbd = [N = 0](const complex_t& c) mutable
                {
                    return N = !N;
                };
    /* clang-format on */

    std::copy_if(x.begin(), x.end(), even.begin(), lmbd);
    std::copy_if(x.begin() + 1, x.end(), odd.begin(), lmbd);

    // conquer
    FFT(even);
    FFT(odd);

    // combine
    for(size_t k = 0; k < x.size() / 2; ++k)
    {
        complex_t t  = std::polar(1.0, -2 * std::numbers::pi * k / x.size()) * odd[k];
        x[k]         = even[k] + t;
        x[k + x.size() / 2] = even[k] - t;
    }
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
