#include "Fonction.h"
#include <algorithm>

void FFT(std::vector<std::complex<double>>& x)
{
    const size_t N = x.size();
    if(N <= 1)
        return;

    // divide
    std::vector<std::complex<double>> even(x.size() % 2 == 0 ? x.size() / 2 : x.size() / 2 + 1);
    std::vector<std::complex<double>> odd(x.size() % 2 == 0 ? x.size() / 2 : x.size() / 2 + 1);

    std::copy_if(x.begin(), x.end(), even.begin(), [N = 0](const Complex& c) mutable {
        return N = !N;
    });
    std::copy_if(x.begin() + 1, x.end(), odd.begin(), [N = 0](const Complex& c) mutable {
        return N = !N;
    });
    // CArray even{ = x[std::slice(0, N / 2, 2)]};
    // CArray odd  = x[std::slice(1, N / 2, 2)];

    // conquer
    FFT(even);
    FFT(odd);

    // combine

    for(size_t k = 0; k < N / 2; ++k)
    {
        Complex t    = std::polar(1.0, -2 * PI * k / N) * odd[k];
        x[k]         = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}

void MM(CArray& sample, int size, int nsample[])
{
    for(size_t i = 0; i < size; i++)
    {
        nsample[i] =
          sqrt((sample[0].imag() * sample[0].imag()) + (sample[0].real() * sample[0].real()));
    }
    for(int i = 1; i < size - 2; i++)
    {
        nsample[i] = (nsample[i - 1] + nsample[i] + nsample[i + 1]);
    }
}

int peek(int sample[], int size)
{
    int max = sample[0];
    int pos = 0;
    for(size_t i = 0; i < size; i++)
    {
        if(max < sample[i])
        {
            max = sample[i];
            pos = i;
        }
    }
    return pos;
}
