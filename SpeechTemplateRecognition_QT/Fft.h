#pragma once
#include <complex>
#include <iostream>
#include <valarray>

const double PIv = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

class Fft
{
public:
	static void fft(CArray& x);
	static void fft2(CArray& x);
	static void ifft(CArray& x);
};

