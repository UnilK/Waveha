#pragma once

#include "designg/rbuffer.h"
#include "designg/wavelet.h"

namespace designg {

class Pitcher {

public:

    Pitcher(int framerate, float minPitchHZ);

    float process(float sample);

    void set_absolute_pitch_shift(float shift);

    void set_color_shift(const std::vector<float> &new_shifts);

    int get_shift_size();

    int get_delay();

private:

    void calc_scale();

    double fpointer, shift, scale;
    int ipointer, min, max, pop, bsize;
    int calc_state, calc_period;

    std::vector<Wavelet > wavelet;
    std::vector<float> shifts;

    rbuffer<float> ibuff, obuff;

};

}

