#pragma once

#include <complex>
#include <vector>

namespace math {

// Laplace transform... I guess
std::complex<float> lt(const float *waves, unsigned size, float frequency);
std::complex<float> lt(const std::vector<float> &waves, float frequency);

// Poor man's DFT. can't handle vectors larger than 2^15
std::vector<std::complex<float> > ft(
        const float *waves, unsigned size, unsigned n, bool haszero = 0);

std::vector<std::complex<float> > ft(
        const std::vector<float> &waves, unsigned n, bool haszero = 0);

std::vector<float> ift(
        const std::vector<std::complex<float> > &frequencies,
        unsigned size, bool haszero = 0);

struct FTPrecalc {
    FTPrecalc();
    static const unsigned N = 1<<18;
    std::complex<float> exp[N+1];
};

extern FTPrecalc ftPrecalc;
std::complex<float> cexp(double radians);

}

