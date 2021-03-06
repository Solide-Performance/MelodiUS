#pragma once
#include <complex>
#include <iostream>
#include <cmath>
#include <vector>

constexpr double PI = 3.141592653589793238460;
using Complex       = std::complex<double>;
using CArray        = std::vector<Complex>;

void FFT(std::vector<std::complex<double>>& x);
void MM(CArray& sample, int size);
int  peek(int sample[], int size);
