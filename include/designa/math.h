#pragma once

#include <array>
#include <complex>
#include <vector>

namespace designa {

float rnd(float);

void in_place_fft(
        std::vector<std::complex<float> > &v,
        bool inv = 0);

std::vector<std::complex<float> > fft(const std::vector<float> &v);

std::array<std::vector<std::complex<float> >, 2> fft(
        const std::vector<float> &a,
        const std::vector<float> &b);

std::vector<float> inverse_fft(
        const std::vector<std::complex<float> > &v);

std::vector<float> energy_mse(const std::vector<float> &a, const std::vector<float> &b);

std::vector<float> sin_window(int length);

std::vector<float> cos_window(int length);

float energy_frequency(std::vector<float> frequency_energy);

std::complex<float> unit_complex(const std::complex<float> &c);

}

