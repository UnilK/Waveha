#pragma once

#include "designh/rbuffer.h"
#include "designh/enveloper.h"
#include "designh/rsegtree.h"

#include <complex>
#include <vector>

namespace designh {

struct Wavelet1 {

    Wavelet1();
    Wavelet1(int length, float spins, float gain);

    std::complex<float> eval(float t);

    float frequency_response(float framerate, float frequency);
    
    void update_shift(float target);

    int length, state;
    float spins, gain;
    std::vector<std::complex<float> > w;

    double fpointer, target_shift, shift;
    int ipointer;
};

class Pitcher1 {

public:

    Pitcher1(int framerate, float minPitchHZ);

    float process(float sample);

    void set_absolute_pitch_shift(float shift);

    void set_color_shift(float shift);

    int get_delay();

private:

    Enveloper enveloper;
    rbuffer<float> delayer;
    rsegtree energyquery;

    void calc_scale();

    void lock_wavelets();

    float similarity;
    double fpointer, shift, scale;
    int ipointer, min, max, pop, bsize, maxlen;
    int calc_state, calc_period;

    int no_jump;

    std::vector<Wavelet1 > wavelet;

    rbuffer<float> ibuff, obuff, ebuff;

};

std::vector<Wavelet1> hodgepodge(std::string version);

}

