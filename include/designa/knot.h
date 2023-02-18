#pragma once

#include "designa/pacer.h"
#include "designa/ftip.h"
#include "designa/scolor.h"
#include "designa/equalizer.h"
#include "designa/binder.h"

#include <valarray>
#include <vector>
#include <deque>

namespace designa {

class Knot {

public:

    Knot(int frame_rate = 44100, float min_frequency = 60.0f, int equalizer_size = 256);

    float process(float sample);

    int get_delay();

    // pitch shift
    void set_absolute_pitch_shift(float shift);
    
    // color shift
    int get_color_shift_size();
    void set_color_shift(const std::vector<float> &new_shift);

    // equalization
    void enable_eq_merge(bool);
    void enable_eq_gain(bool);
    void enable_eq_clean(bool);
    void enable_eq_noise(bool);
    void enable_eq_shuffle(bool);

    void set_eq_merge(const std::vector<float> &new_merge);
    void set_eq_gain(const std::vector<float> &new_gain);
    void set_eq_clean(const std::vector<float> &new_clean);
    void set_eq_noise(const std::vector<float> &new_noise);
    void set_eq_shuffle(const std::vector<float> &new_shuffle);

    int get_eq_frequency_window_size();

    // value binding
    void set_bind_threshold(float amplitude);

private:

    Pacer pacer;
    Ftip ftip;
    Scolor scolor;
    Equalizer equalizer;
    Binder binder;

    std::deque<float> shift_pipe, delay_pipe;

};

}

