#pragma once

#include <array>
#include <complex>
#include <vector>

namespace designd {

void in_place_fft(
        std::vector<std::complex<float> > &v,
        bool inv = 0);

std::vector<std::complex<float> > fft(
        const std::vector<float> &v);

std::array<std::vector<std::complex<float> >, 2> fft(
        const std::vector<float> &a,
        const std::vector<float> &b);

std::vector<float> inverse_fft(
        const std::vector<std::complex<float> > &v);

std::array<std::vector<float>, 2> inverse_fft(
        std::vector<std::complex<float> > a,
        std::vector<std::complex<float> > b);

std::vector<float> energy_mse(
        const std::vector<float> &a,
        const std::vector<float> &b);

std::vector<float> padded_energy_mse(
        const std::vector<float> &a,
        const std::vector<float> &b,
        int border);

}

