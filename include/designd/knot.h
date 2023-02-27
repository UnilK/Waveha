#pragma once

#include "designd/pacer.h"
#include "designd/resampler.h"
#include "designd/painter.h"
#include "designd/equalizer.h"
#include "designd/binder.h"

#include <valarray>
#include <vector>
#include <deque>

namespace designd {

class Knot {

public:

    Knot(   int frameRate = 44100,
            float minPitchHZ = 60.0f,
            int painterSize = 256,
            int equalizerSize = 256);

    float process(float sample);

    int get_delay();

    // pitch shift
    void set_absolute_pitch_shift(float shift);
    
    // color shift
    int get_color_shift_size();
    void set_color_shift(const std::vector<float> &new_shift);

    // equalization
    void enable_eq_gain(bool);
    void enable_eq_clean(bool);
    void enable_eq_noise(bool);
    void enable_eq_shuffle(bool);
    void enable_eq_blur(bool);

    void set_eq_gain(const std::vector<float> &new_gain);
    void set_eq_clean(const std::vector<float> &new_clean);
    void set_eq_noise(const std::vector<float> &new_noise);
    void set_eq_shuffle(const std::vector<float> &new_shuffle);
    void set_eq_blur(const std::vector<float> &new_blur);

    int get_eq_frequency_window_size();

    // value binding
    void set_bind_threshold(float amplitude);

private:

    Pacer pacer;
    Resampler resampler;
    Painter painter;
    Equalizer equalizer;
    Binder binder;

    std::deque<float> resamplerPipe;

};

}

