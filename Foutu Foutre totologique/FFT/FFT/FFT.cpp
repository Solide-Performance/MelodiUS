// FFT.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Fonction.h"
#include <algorithm>
#include <cmath>
#include <iostream>

int main()
{
    constexpr int    nombre = 8;
    constexpr double freq   = 100;

    const Complex test[nombre] = {1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0};

    /*static Complex test[nombre] = {0.0f, 0.0f};
    for(int i = 0; i < nombre; i++)
    {
        test[i] = sin(2. * PI * freq * i);
    }*/

    CArray data(&test[0], &test[nombre]);

    FFT(data);
    std::cout << "fft" << std::endl;
    for(int i = 0; i < nombre; ++i)
    {
        std::cout << std::abs(data[i]) << std::endl;
    }


    // https://stackoverflow.com/a/4230658/10827197
    int firstPeak = std::distance(
      data.begin(),
      std::max_element(data.begin() + 1, data.end(), [](const Complex& c1, const Complex& c2) {
          return std::abs(c1) < std::abs(c2);
      }));

    std::cout << "freq" << data[firstPeak].real() << std::endl;
}
