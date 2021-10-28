#define pragma once

#include <cstdint>
#include <complex>
#include <vector>

namespace wave{
    
float match_pitch(
        std::vector<std::complex<float> > &spectrum,
        int32_t dataFrequency,
        float frequency,
        int32_t peaks);
float match_pitch(
        std::complex<float> *spectrum,
        int32_t size,
        int32_t dataFrequency,
        float frequency,
        int32_t peaks);

float find_pitch(
        std::vector<float> &waves,
        int32_t dataFrequency,
        float minFreq,
        float maxFreq,
        int32_t peaks);
float find_pitch(
        float *waves,
        int32_t size,
        int32_t dataFrequency,
        float minFreq,
        float maxFreq,
        int32_t peaks);

int32_t best_pitch_length(
        std::vector<float> &waves,
        int32_t length_approx,
        float tolerance); 
int32_t best_pitch_length(
        float *waves,
        int32_t size,
        int32_t length_approx,
        float tolerance);

std::vector<float> pitch_match_graph(
        std::vector<float> &waves,
        int32_t dataFrequency,
        float minFreq,
        float maxFreq,
        int32_t peaks);
std::vector<float> pitch_match_graph(
        float *waves,
        int32_t size,
        int32_t dataFrequency,
        float minFreq,
        float maxFreq,
        int32_t peaks);

std::vector<float> wave_correlation_graph(
        std::vector<float> &waves,
        int32_t length_approx,
        float tolerance); 
std::vector<float> wave_correlation_graph(
        float *waves,
        int32_t size,
        int32_t length_approx,
        float tolerance);

}

