#include "designa/knot.h"
#include "designa/math.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace designa {

Knot::Knot(int frame_rate, float min_frequency, int equalizer_size) :
    pacer(frame_rate, min_frequency, 1200.0f),
    ftip(16),
    scolor(frame_rate, min_frequency, 0.1f, 8),
    equalizer(equalizer_size)
{
    ftip.set_shift(1.0f);
    
    int process_delay = pacer.get_delay() + scolor.get_delay() + ftip.get_delay();
    for(int i=0; i<process_delay - 2; i++) delay_pipe.push_back(0.0f);
    
}

float Knot::process(float sample){
  
    pacer.push(sample);
    delay_pipe.push_back(sample);

    while(shift_pipe.empty()){
        auto p = ftip.process(scolor.process(pacer.pull() + rnd(1e-6f), pacer.get_scale()));
        for(float j : p) shift_pipe.push_back(j);
    }
    
    float processed = shift_pipe.front();
    float raw = delay_pipe.front();

    processed = equalizer.process(processed, raw);
    processed = binder.process(processed);

    shift_pipe.pop_front();
    delay_pipe.pop_front();

    return processed;
}
    
int Knot::get_delay(){
    return pacer.get_delay() + scolor.get_delay() + ftip.get_delay() + equalizer.get_delay();
}

void Knot::set_absolute_pitch_shift(float shift){
    shift = std::max(0.05f, std::min(shift, 20.0f));
    ftip.set_shift(shift);
}

int Knot::get_color_shift_size(){
    return scolor.get_shift_size();
}

void Knot::set_color_shift(const std::vector<float> &new_shift){
    scolor.set_shift(new_shift);
}
    
void Knot::enable_eq_merge(bool x){
    equalizer.enable_merge(x);
}

void Knot::enable_eq_gain(bool x){
    equalizer.enable_gain(x);
}

void Knot::enable_eq_clean(bool x){
    equalizer.enable_clean(x);
}

void Knot::enable_eq_noise(bool x){
    equalizer.enable_noise(x);
}

void Knot::enable_eq_shuffle(bool x){
    equalizer.enable_shuffle(x);
}

void Knot::set_eq_merge(const std::vector<float> &new_merge){
    equalizer.set_merge(new_merge);
}

void Knot::set_eq_gain(const std::vector<float> &new_gain){
    equalizer.set_gain(new_gain);
}

void Knot::set_eq_clean(const std::vector<float> &new_clean){
    equalizer.set_clean(new_clean);
}

void Knot::set_eq_noise(const std::vector<float> &new_noise){
    equalizer.set_noise(new_noise);
}
void Knot::set_eq_shuffle(const std::vector<float> &new_shuffle){
    equalizer.set_shuffle(new_shuffle);
}

int Knot::get_eq_frequency_window_size(){
    return equalizer.get_frequency_size();
}

void Knot::set_bind_threshold(float amplitude){
    binder.set_bound(amplitude);
}

}

