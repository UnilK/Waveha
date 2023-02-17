#include "designa/knot.h"
#include "designa/color.h"
#include "designa/math.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace designa {

Knot::Knot(int frame_rate, float min_frequency) :
    pacer(frame_rate, min_frequency, 1200.0f),
    ftip(16),
    scolor(frame_rate, min_frequency, 0.1f)
{
    ftip.set_shift(1.0f);
    
    absolute_pitch_shift = 1.0f;
    
    process_delay = pacer.get_delay() + scolor.get_delay() + ftip.get_delay();
    for(int i=0; i<process_delay - 2; i++) delay_pipe.push_back(0.0f);
    
}

float Knot::process(float sample){
  
    pacer.push(sample);
    delay_pipe.push_back(sample);

    while(shift_pipe.empty()){
        for(float j : ftip.process(scolor.process(pacer.pull(), pacer.get_scale()))){
            shift_pipe.push_back(j);
        }
    }
    
    float processed = shift_pipe.front();
    // float raw = delay_pipe.front();

    shift_pipe.pop_front();
    delay_pipe.pop_front();

    return processed;
}

void Knot::set_absolute_pitch_shift(float shift){
    absolute_pitch_shift = std::max(0.05f, std::min(shift, 20.0f));
    ftip.set_shift(absolute_pitch_shift);
}

int Knot::get_color_shift_size(){
    return scolor.get_shift_size();
}

void Knot::set_color_shift(const std::vector<float> &new_shift){
    scolor.set_shift(new_shift);
}

}

