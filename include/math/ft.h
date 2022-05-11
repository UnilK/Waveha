#pragma once

#include <complex>
#include <vector>

namespace math {

// Laplace transform... I guess
std::complex<float> lt(const float *waves, unsigned size, float frequency);
std::complex<float> lt(const std::vector<float> &waves, float frequency);

// Poor man's Fourier Transform: performs dft on the first n
// frequencies (0/constant not included) and returns those.
std::vector<std::complex<float> > ft(const float *waves, unsigned size, unsigned n);
std::vector<std::complex<float> > ft(const std::vector<float> &waves, unsigned n);
std::vector<float> ift(const std::vector<std::complex<float> > &frequencies, unsigned size);

struct FTPrecalc {
    FTPrecalc();
    static const unsigned N = 1<<17;
    std::complex<float> exp[N+1];
};

extern FTPrecalc ftPrecalc;
std::complex<float> cexp(double radians);

}

