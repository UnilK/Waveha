#pragma once

#include <vector>
#include <complex>

namespace ml { class Stack; }

namespace change {

struct PeakMatchVars {
    float minF = 80, maxF = 800;
    int peaks = 10;
    float exponent = 2;
};

extern PeakMatchVars defaultPeakVars;

std::vector<float> peak_match_graph(
        const std::vector<float> &audio,
        PeakMatchVars = defaultPeakVars);

std::vector<float> phase_graph(
        const std::vector<float> &audio,
        unsigned period);

std::vector<float> random_experiment(const std::vector<float> &audio);
std::vector<float> random_experiment2(const std::vector<float> &audio, float low, float high);

std::vector<std::complex<float> > candom_experiment(const std::vector<float> &audio);

struct CorrelationVars {
    float exponent = 3;
    bool sign = 1;
};

extern CorrelationVars defaultCorrelationVars;

std::vector<float> correlation_graph(
        const std::vector<float> &audio,
        CorrelationVars = defaultCorrelationVars);

std::vector<float> ml_graph(ml::Stack *stack,
        const std::vector<float> &audio, float pitch);

std::vector<float> pitch_changer(std::vector<float> audio, float pitch);

std::vector<std::complex<float> > translate(
        const std::vector<std::complex<float> > &frequencies,
        float pitch_shift);


}

