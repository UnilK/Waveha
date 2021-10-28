#pragma once

#include <cstdint>
#include <complex>
#include <vector>

namespace math{
      
// calculate discrete fourier transform of v. only sizes
// that are powers of 2 are supported.
bool in_place_fft(std::complex<float> *v, const int32_t n, bool inv = 0);
std::vector<std::complex<float> > fft(float *v, const int32_t n);

bool in_place_fft(std::vector<std::complex<float> > &v, bool inv = 0);
std::vector<std::complex<float> > fft(std::vector<std::complex<float> > v, bool inv = 0);
std::vector<std::complex<float> > fft(std::vector<float> &v);
    	
}

