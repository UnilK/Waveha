#pragma once

#include "designh/rbuffer.h"
#include "designh/enveloper.h"
#include "designh/rsegtree.h"

#include <complex>
#include <vector>

namespace designh {

struct Wavelet2 {

    Wavelet2();
    Wavelet2(int length, float spins, float gain);

    std::complex<float> eval(float t);

    float frequency_response(float framerate, float frequency);
    
    void update_shift(float target);

    int length, state;
    float spins, gain;
    std::vector<std::complex<float> > w;

    double fpointer, target_shift, shift;
    int ipointer;
};

class Pitcher2 {

public:

    Pitcher2(int framerate, float minPitchHZ);

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

    std::vector<Wavelet2 > wavelet;

    rbuffer<float> ibuff, obuff, ebuff;

};

std::vector<Wavelet2> hodgepodge2(std::string version);

}

