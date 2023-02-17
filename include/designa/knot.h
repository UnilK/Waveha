#pragma once

#include "designa/pacer.h"
#include "designa/ftip.h"
#include "designa/scolor.h"

#include <valarray>
#include <vector>
#include <deque>

namespace designa {

class Knot {

public:

    Knot(int frame_rate = 44100, float min_frequency = 60.0f);

    float process(float sample);

    void set_absolute_pitch_shift(float shift);
    
    int get_color_shift_size();
    
    void set_color_shift(const std::vector<float> &new_shift);

private:

    Pacer pacer;
    Ftip ftip;
    Scolor scolor;

    float absolute_pitch_shift;

    int process_delay;
    std::deque<float> shift_pipe, delay_pipe;

};

}

