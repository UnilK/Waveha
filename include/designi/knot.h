#pragma once

#include "designi/pitcher1.h"
#include "designi/equalizer.h"
#include "designi/binder.h"

#include <valarray>
#include <vector>
#include <deque>

namespace designi {

class Knot {

public:

    Knot(   int frameRate = 44100,
            float minPitchHZ = 60.0f,
            int equalizerSize = 256);

    float process(float sample);

    int get_delay();

    void enable_pitcher(bool);
    void enable_equalizer(bool);
    void enable_binder(bool);

    // pitch shift
    void set_absolute_pitch_shift(float shift);
    void set_color_shift(float shift);

    // equalization
    void enable_eq_gain(bool);
    void enable_eq_clean(bool);

    void set_eq_gain(const std::vector<float> &new_gain);
    void set_eq_clean(const std::vector<float> &new_clean);

    int get_eq_frequency_window_size();

    // value binding
    void set_bind_threshold(float amplitude);

private:

    bool pitcher_x, equalizer_x, binder_x;

    Pitcher1 pitcher;
    Equalizer equalizer;
    Binder binder;

};

}

