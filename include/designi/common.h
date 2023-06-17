#pragma once

#include <complex>
#include <vector>

namespace designi {

float rnd(const float &d);

int rnd_int(int min, int max);

std::complex<float> unit_complex(const std::complex<float> &c);

std::vector<float> cos_window(int length);

std::vector<std::complex<float> > cos_wavelet(int length, double spins);

std::vector<std::complex<float> > extract_phase(
        const std::vector<std::complex<float> > &frequency);

std::vector<float> extract_energy(
        const std::vector<std::complex<float> > &frequency);

std::vector<std::complex<float> > create_frequency(
        const std::vector<float> &energy,
        std::vector<std::complex<float> > phase);

}

