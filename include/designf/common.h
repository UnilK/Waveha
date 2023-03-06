#pragma once

#include <complex>
#include <vector>

namespace designf {

float rnd(const float &d);

std::complex<float> unit_complex(const std::complex<float> &c);

std::vector<float> cos_window(int length);

std::vector<std::complex<float> > cos_wavelet(int length, double spins);

float maximum_peak(const std::vector<float> &v, int min, int max);

std::vector<std::complex<float> > extract_phase(
        const std::vector<std::complex<float> > &frequency);

std::vector<float> extract_energy(
        const std::vector<std::complex<float> > &frequency);

std::vector<std::complex<float> > create_frequency(
        const std::vector<float> &energy,
        std::vector<std::complex<float> > phase);

std::vector<float> shift_bins(
        const std::vector<float> &bin,
        const std::vector<float> &shift);

}


