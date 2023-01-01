#pragma once

#include <complex>
#include <vector>
#include <array>

namespace math {

// calculate discrete fourier transform of v. works fast
// for sizes that are powers of 2. otherwise falls back to bluestein -> 6x slowdown.

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

// correlation[i] = elementwise sum of matching a * (b shifted to the right by i).
std::vector<float> correlation(
        const std::vector<float> &a,
        const std::vector<float> &b,
        unsigned size = 0);

// vector a is packed to real axis, b to imaginary axis.
void in_place_correlation(std::vector<std::complex<float> > &v);

// mse[i] = sum of elementwise matching squared deviations of a and (b shifted to the right by i).
std::vector<float> mse(
        const std::vector<float> &a,
        const std::vector<float> &b);

// cumulative sum normalized mse
std::vector<float> nmse(
        const std::vector<float> &a,
        const std::vector<float> &b);

// implementation that utilizes fft's bandwidth of 2 vectors. The second vector is optional.
std::array<std::vector<float>, 2> autocorrelation(std::vector<float> a, std::vector<float> b = {});

// these are ~6 times slower on average than radix 2, but support all sizes.

std::vector<std::complex<float> > bluestein(std::vector<std::complex<float> > v, bool inv = 0); 
std::vector<std::complex<float> > bluestein(std::vector<float> &v);
std::vector<float> inverse_bluestein(std::vector<std::complex<float> > &v); 

// default precalc table size is 18 -> vectors of size 2^18 max can be processed

struct FFTPrecalc {

    FFTPrecalc(unsigned);
    void resize(unsigned);

    unsigned B;

    std::vector<std::vector<std::complex<float> > > w;
    std::vector<unsigned> invbit;

};

extern FFTPrecalc fftPrecalc;

}

