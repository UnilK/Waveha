#pragma once

#include <complex>
#include <vector>

namespace designb {

float rnd(const float &d);

std::complex<float> unit_complex(const std::complex<float> &c);

std::vector<float> cos_window(int length);

std::vector<std::complex<float> > cos_wavelet(int length, double spins);

float maximum_peak(const std::vector<float> &v, int min, int max);

}

