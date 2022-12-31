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

// multiplies the waves by a cosine window (convolution kernel [0.25, 0.5, 0.25] on frequency side)
// and then takes dft of every other frequency
std::vector<std::complex<float> > cos_window_ft(
        float *waves, unsigned size, unsigned n, bool haszero = 0);
std::vector<std::complex<float> > cos_window_ft(
        std::vector<float> waves, unsigned n, bool haszero = 0);

std::vector<std::complex<float> > precise_ft(
        const std::vector<float> &waves, unsigned n, bool haszero = 0, float speed = 1.0f);

std::vector<float> ift(
        const std::vector<std::complex<float> > &frequencies,
        unsigned size, bool haszero = 0);

std::vector<float> sized_ift(
        const std::vector<std::complex<float> > &frequencies,
        unsigned period, unsigned size);

struct FTPrecalc {
    FTPrecalc();
    static const unsigned N = 1<<18;
    static constexpr double dN = N;
    std::complex<float> exp[N+1];
};

extern FTPrecalc ftPrecalc;
std::complex<float> cexp(double radians);

struct DFTPrecalc {
    DFTPrecalc();
    static const unsigned N = 1<<18;
    static constexpr double dN = N;
    std::vector<std::complex<float> > exp;
};

extern DFTPrecalc dftPrecalc;
std::complex<float> dexp(double radians);
}

