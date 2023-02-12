#include "designa/knot.h"

#include <cmath>

namespace designa {

Knot::Knot(int frame_rate, float min_frequency) :
    pace(frame_rate, min_frequency, 1200.0f),
    ftip(16),
    frame_rate(frame_rate)
{
    ftip.set_shift(1.0f);
    
    pitch_shift_delay = pace.get_delay() + ftip.get_delay();
    
    pitch_shift_x = 1;
    pitch_correction_x = 0;

    absolute_pitch_shift = 1.0f;
    pitch_correction_shift = 1.0f;
    
    set_pitch_correction_frequency(440.0f);
    set_pitch_correction_scale(8);
    set_pitch_correction_power(1.0f);
    
    for(int i=0; i<pitch_shift_delay; i++) delay_pipe.push_back(0.0f);
}

float Knot::process(float sample){
    
    float raw_sample = sample;
    float pitch_shifted_sample = sample;

    if(pitch_shift_x){
        
        delay_pipe.push_back(sample);
        pace.push(sample);
    
        if(pitch_correction_x){

            float original = (float)frame_rate / pace.get_scale();
            float current = original * absolute_pitch_shift;
            float target = pitch_correction_frequency;
            const float &step = pitch_correction_scale_step;

            while(std::abs(current - target * 2) < std::abs(current - target)) target *= 2;
            while(std::abs(current - target / 2) < std::abs(current - target)) target /= 2;
            while(std::abs(current - target * step) < std::abs(current - target)) target *= step;
            while(std::abs(current - target / step) < std::abs(current - target)) target /= step;

            const float &power = pitch_correction_power;
            const float new_shift = target / original;
            pitch_correction_shift = pitch_correction_shift * (1.0f - power) + new_shift * power;

            ftip.set_shift(pitch_correction_shift);
        }

        while(shift_pipe.empty()){
            for(float j : ftip.process(pace.pull())){
                shift_pipe.push_back(j);
            }
        }
        
        raw_sample = delay_pipe.front();
        pitch_shifted_sample = shift_pipe.front();
        
        delay_pipe.pop_front();
        shift_pipe.pop_front();
    }

    float processed = pitch_shifted_sample;

    return processed;
}

void Knot::enable_pitch_shift(bool x){
    
    pitch_shift_x = x;

    if(!x){
        shift_pipe.clear();
        for(float &i : delay_pipe) i = 0.0f;
    }
}

void Knot::set_absolute_pitch_shift(float shift){
    absolute_pitch_shift = std::max(0.05f, std::min(shift, 20.0f));
    ftip.set_shift(absolute_pitch_shift);
}

void Knot::enable_pitch_correction(bool x){
    pitch_correction_x = x;
}

void Knot::set_pitch_correction_frequency(float frequency){
    pitch_correction_frequency = frequency;
}

void Knot::set_pitch_correction_scale(int scale){
    pitch_correction_scale = std::max(1, std::min(scale, 32));
    pitch_correction_scale_step = std::pow(2.0f, 1.0f / pitch_correction_scale);
}

void Knot::set_pitch_correction_power(float power){
    pitch_correction_power = std::max(0.0f, std::min(power, 1.0f));
}

}

