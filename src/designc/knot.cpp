#include "designc/knot.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace designc {

Knot::Knot( int frameRate,
            float minPitchHZ,
            int painterSize,
            int equalizerSize) : 
    pacer(frameRate, minPitchHZ, 1200.0f),
    resampler(16),
    painter(painterSize),
    equalizer(equalizerSize)
{
    resampler.set_shift(1.0f);
}

float Knot::process(float sample){
  
    pacer.push(sample);

    while(resamplerPipe.empty()){
        auto p = resampler.process(painter.process(pacer.pull()));
        for(float j : p) resamplerPipe.push_back(j);
    }
    
    float processed = resamplerPipe.front();
    resamplerPipe.pop_front();

    processed = equalizer.process(processed);
    
    processed = binder.process(processed);

    return processed;
}
    
int Knot::get_delay(){
    return pacer.get_delay() + painter.get_delay() + resampler.get_delay() + equalizer.get_delay();
}

void Knot::set_absolute_pitch_shift(float shift){
    shift = std::max(1.0f / 8, std::min(shift, 8.0f));
    resampler.set_shift(shift);
}

int Knot::get_color_shift_size(){
    return painter.get_shift_size();
}

void Knot::set_color_shift(const std::vector<float> &new_shift){
    painter.set_shift(new_shift);
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

