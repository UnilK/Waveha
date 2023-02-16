#pragma once

#include <valarray>
#include <vector>
#include <complex>

namespace designa {

std::vector<float> pick_bit(
        const std::valarray<float> &buffer,
        int offset, int size);

void add_bit(
        const std::vector<float> &bit,
        std::valarray<float> &buffer,
        int offset);

void apply_window(
        std::vector<float> &bit,
        const std::vector<float> &window);

std::vector<std::complex<float> > frequency_phases(
        const std::vector<std::complex<float> > &frequency);

std::vector<float> frequency_energies(
        const std::vector<std::complex<float> > &frequency);

std::vector<std::complex<float> > join_energy_to_phase(
        const std::vector<float> &energy,
        std::vector<std::complex<float> > phase);

void add_phase_noise(
        std::vector<std::complex<float> > &phase,
        const std::vector<float> &noise_magnitude);

std::vector<float> split_to_bins(
        const std::vector<float> &energy,
        float bin_size);

std::vector<float> shift_bins(
        const std::vector<float> &bin,
        const std::vector<float> &shift);

std::vector<float> color_injection(
        const std::vector<float> &source_color,
        const std::vector<float> &source_pattern,
        const std::vector<float> &color_shift,
        float color_step,
        float pattern_step);

}

