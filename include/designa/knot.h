#pragma once

#include "designa/pace.h"
#include "designa/ftip.h"

#include <valarray>
#include <vector>
#include <deque>

namespace designa {

class Knot {

public:

    Knot(int frame_rate = 44100, float min_frequency = 60.0f);

    float process(float sample);

    void set_absolute_pitch_shift(float shift);
    
    void enable_pitch_correction(bool);
    void set_pitch_correction_frequency(float frequency);
    void set_pitch_correction_scale(int scale);
    void set_pitch_correction_power(float power);

    void enable_color_manipulation(bool);

private:

    Pace pace;
    Ftip ftip;

    int frame_rate;

    float absolute_pitch_shift;
    
    bool pitch_correction_x;
    float pitch_correction_shift;
    float pitch_correction_power;
    float pitch_correction_frequency;
    float pitch_correction_scale;
    float pitch_correction_scale_step;

    int pitch_shift_delay;
    std::deque<float> shift_pipe, delay_pipe;
    
    int window_size;
    int buffer_size;
    int buffer_pointer;
    int window_state;
    int previous_window_width;

    std::valarray<float> shifted_buffer;
    std::valarray<float> original_buffer;
    std::valarray<float> recolored_buffer;
    std::valarray<int> wave_period;

};

}

