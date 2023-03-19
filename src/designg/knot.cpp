#include "designg/knot.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace designg {

Knot::Knot( int frameRate,
            float minPitchHZ,
            int equalizerSize) : 
    pitcher(frameRate, minPitchHZ),
    equalizer(equalizerSize)
{
    pitcher_x = 1;
    equalizer_x = 1;
    binder_x = 1;
}

float Knot::process(float sample){
  
    if(pitcher_x) sample = pitcher.process(sample);
    
    if(equalizer_x) sample = equalizer.process(sample);
    
    if(binder_x) sample = binder.process(sample);

    return sample;
}
    
int Knot::get_delay(){
    
    int delay = 0;

    if(pitcher_x) delay += pitcher.get_delay();
    
    if(equalizer_x) delay += equalizer.get_delay();
    
    if(binder_x) delay += binder.get_delay();

    return delay;
}

void Knot::enable_pitcher(bool x){ pitcher_x = x; }

void Knot::enable_equalizer(bool x){ equalizer_x = x; }

void Knot::enable_binder(bool x){ binder_x = x; }

void Knot::set_absolute_pitch_shift(float shift){
    pitcher.set_absolute_pitch_shift(shift);
}

void Knot::set_color_shift(const std::vector<float> &shift){
    pitcher.set_color_shift(shift);
}

void Knot::enable_eq_gain(bool x){
    equalizer.enable_gain(x);
}

void Knot::enable_eq_clean(bool x){
    equalizer.enable_clean(x);
}

void Knot::set_eq_gain(const std::vector<float> &new_gain){
    equalizer.set_gain(new_gain);
}

void Knot::set_eq_clean(const std::vector<float> &new_clean){
    equalizer.set_clean(new_clean);
}

int Knot::get_eq_frequency_window_size(){
    return equalizer.get_frequency_size();
}

int Knot::get_color_shift_size(){
    return pitcher.get_shift_size();
}

void Knot::set_bind_threshold(float amplitude){
    binder.set_bound(amplitude);
}

}

