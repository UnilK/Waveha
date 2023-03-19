#pragma once

#include <complex>
#include <vector>

namespace designg {

struct Wavelet {

    Wavelet();
    Wavelet(int length, float spins, float gain);

    std::complex<float> eval(float t);

    float frequency_response(float framerate, float frequency);

    int length, state;
    float spins, gain, phase, amplitude;
    bool increasing;
    std::vector<std::complex<float> > w;
};

std::vector<Wavelet> hpset(std::string version);


}

