#pragma once
#include <complex>
#include <iostream>
#include <valarray>
#include <math.h>
const double                   PI = 3.141592653589793238460;
typedef std::complex<double>   Complex;
typedef std::valarray<Complex> CArray;

void FFT(CArray& x);
void MM(CArray& sample,int size);
int  peek(int sample[], int size);


