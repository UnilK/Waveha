#pragma once

#include <cstdint>
#include <complex>
#include <vector>

namespace math {

// calculate discrete fourier transform of v. only sizes
// that are powers of 2 are supported.

void in_place_fft(std::complex<float> *v, unsigned n, bool inv = 0);
void in_place_fft(std::vector<std::complex<float> > &v, bool inv = 0);

std::vector<std::complex<float> > fft(const float *v, unsigned n);
std::vector<std::complex<float> > fft(const std::vector<float> &v);

std::vector<float> inverse_fft(const std::complex<float> *v, unsigned n);
std::vector<float> inverse_fft(const std::vector<std::complex<float> > &v);

std::vector<std::complex<float> > fft(std::vector<std::complex<float> > v, bool inv = 0);
std::vector<std::complex<float> > fft(std::complex<float> *v, unsigned n, bool inv = 0);

std::vector<float> convolution(std::vector<float> &a, std::vector<float> &b, unsigned size = 0);
std::vector<std::complex<float> > convolution(
        std::vector<std::complex<float> > a,
        std::vector<std::complex<float> > b,
        unsigned size = 0);

std::vector<float> correlation(std::vector<float> a, std::vector<float> b, unsigned size = 0);

// these are ~6 times slower on average than radix 2, but support all sizes.

std::vector<std::complex<float> > bluestein(std::vector<std::complex<float> > v, bool inv = 0); 
std::vector<std::complex<float> > bluestein(std::vector<float> &v);
std::vector<float> inverse_bluestein(std::vector<std::complex<float> > &v); 

// default precalc table size is 18 -> std::vectors of size 2^18 max can be processed

struct FFTPrecalc {

    FFTPrecalc(unsigned);
    void resize(unsigned);

    unsigned B;

    std::vector<std::vector<std::complex<float> > > w;
    std::vector<unsigned> invbit;

};

extern FFTPrecalc fftPrecalc;

}

