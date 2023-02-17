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

float maximum_peak(const std::vector<float> &v,
        int min, int max);

std::vector<float> sin_window(int left, int right);

std::vector<float> cos_window(int left, int right);

std::complex<float> unit_complex(const std::complex<float> &c);

int pow2ceil(int);

}

