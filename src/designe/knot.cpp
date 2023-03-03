#include "designe/knot.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace designe {

Knot::Knot( int frameRate,
            float minPitchHZ,
            int equalizerSize) : 
    pitcher(frameRate, minPitchHZ),
    equalizer(equalizerSize)
{}

float Knot::process(float sample){
  
    sample = pitcher.process(sample);
    
    sample = equalizer.process(sample);
    
    sample = binder.process(sample);

    return sample;
}
    
int Knot::get_delay(){
    return pitcher.get_delay() + equalizer.get_delay();
}

void Knot::set_colored_pitch_shift(float shift){
    pitcher.set_colored_shift(shift);
}

void Knot::set_neutral_pitch_shift(float shift){
    pitcher.set_neutral_shift(shift);
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

void Knot::enable_eq_blur(bool x){
    equalizer.enable_blur(x);
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

void Knot::set_eq_blur(const std::vector<float> &new_blur){
    equalizer.set_blur(new_blur);
}

int Knot::get_eq_frequency_window_size(){
    return equalizer.get_frequency_size();
}

void Knot::set_bind_threshold(float amplitude){
    binder.set_bound(amplitude);
}

}

