// FFT.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Fonction.h"
int main()
{
    int           nombre = 8;
    const Complex test[] = {1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0};
    CArray data(test, nombre);

    FFT(data);
    std::cout << "fft" << std::endl;
    for(int i = 0; i < nombre; ++i)
    {
        std::cout << sqrt((data[i].imag() * data[i].imag()) + (data[i].real() * data[i].real()))
                  << std::endl;
    }
}

