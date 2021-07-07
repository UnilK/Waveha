#ifndef FFT_H
#define FFT_H

#include <cstdint>
#include <complex>

namespace math{
      
    // calculate discrete fourier transform of v. only sizes
    // that are powers of 2 are supported.
    void fft(std::complex<float> *v, int32_t n, bool inv = 0);
    std::complex<float> *fft(float *v, int32_t n, bool inv = 0);
    	
};

#endif
