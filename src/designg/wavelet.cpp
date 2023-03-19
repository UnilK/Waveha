#include "designg/wavelet.h"
#include "designg/common.h"

#include <cmath>
#include <fstream>

namespace designg {

using std::vector;
using std::complex;

Wavelet::Wavelet() :
    length(0),
    state(0),
    spins(0),
    gain(2),
    phase(0),
    amplitude(0),
    increasing(0)
{}

Wavelet::Wavelet(int length, float spins, float gain) : 
    length(length),
    state(0),
    spins(spins),
    gain(gain),
    phase(0),
    amplitude(0),
    increasing(0),
    w(cos_wavelet(length, spins))
{}

std::complex<float> Wavelet::eval(float t){
    return std::complex<float>(1.0f, 2 * M_PI * t * spins / length);
}

float Wavelet::frequency_response(float framerate, float frequency){

    auto sinc = [](float x){
        if(std::abs(x) < 1e-5f) return 1.0f;
        return std::sin(x * (float)M_PI) / (x * (float)M_PI);
    };


    float f0 = framerate / length;
    float fs = f0 * spins;
    
    auto response = [&](float f){
        return 0.25f * sinc((frequency - (f - f0)) / f0)
            + 0.5f * sinc((frequency - f) / f0)
            + 0.25f * sinc((frequency - (f + f0)) / f0);
    };

    float r = response(fs);
    if(frequency > framerate / 4) r += response(framerate - fs);
    else r += response(-fs);

    return r * gain;
}

vector<Wavelet> hpset(std::string version){
    
    std::ifstream I("wavelets/"+version);

    vector<Wavelet> set;

    int len; float freq; float gain;
    while(I >> len){
        I >> freq >> gain;
        set.emplace_back(len, freq, gain);
    }

    return set;
}

}

