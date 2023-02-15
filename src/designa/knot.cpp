#include "designa/knot.h"
#include "designa/color.h"
#include "designa/math.h"

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
    absolute_pitch_shift = 1.0f;
    
    pitch_correction_x = 0;
    pitch_correction_shift = 1.0f;
    
    set_pitch_correction_frequency(440.0f);
    set_pitch_correction_scale(8);
    set_pitch_correction_power(1.0f);
    
    for(int i=0; i<pitch_shift_delay; i++) delay_pipe.push_back(0.0f);

    color_manipulation_x = 1;
    
    window_size = 2 * (int)(frame_rate / min_frequency);
    buffer_size = 8 * window_size;
    buffer_pointer = 0;
    
    window_state = 0;
    window_period = window_size / 2;
    
    shifted_buffer.resize(buffer_size, 0.0f);
    original_buffer.resize(buffer_size, 0.0f);
    recolored_buffer.resize(buffer_size, 0.0f);
}

float Knot::process(float sample){
  
    float processed = sample;
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
    
        processed = pitch_shifted_sample;
    }

    if(color_manipulation_x){
        
        if(buffer_pointer + window_size > buffer_size){
            original_buffer = original_buffer.shift(buffer_pointer);
            shifted_buffer = shifted_buffer.shift(buffer_pointer);
            recolored_buffer = recolored_buffer.shift(buffer_pointer);
            buffer_pointer = 0;
        }

        shifted_buffer[buffer_pointer + window_size - 1] = pitch_shifted_sample + rnd(1e-6f);
        original_buffer[buffer_pointer + window_size - 1] = raw_sample + rnd(1e-6f);

        if(window_state == 0){
            
            int bit_size = 1;
            while(bit_size < window_size) bit_size *= 2;

            auto window = sin_window(window_size);

            auto shifted_bit = pick_window_from_buffer(
                    window, shifted_buffer, buffer_pointer, bit_size);
            auto original_bit = pick_window_from_buffer(
                    window, original_buffer, buffer_pointer, bit_size);

            auto [shifted_freq, original_freq] = fft(shifted_bit, original_bit);
            
            auto shifted_energy = frequency_energies(shifted_freq);
            auto original_energy = frequency_energies(original_freq);
            auto phase_pattern = frequency_phases(original_freq);

            std::vector<float> color_shift(shifted_freq.size(), 1.0f);
            for(unsigned i=0; 8*i < color_shift.size(); i++){
                float d = std::cos(8 * M_PI * i / color_shift.size());
                color_shift[i] = (1.0f - d) + d * absolute_pitch_shift;
            }

            float pitch_step = energy_frequency(original_energy);

            auto new_color = color_injection(original_energy, shifted_energy, color_shift,
                    pitch_step, pitch_step * absolute_pitch_shift);

            std::vector<float> shuffle_magnitudes(phase_pattern.size(), 0.1f);

            add_phase_noise(phase_pattern, shuffle_magnitudes);

            auto output_bit = inverse_fft(join_energy_to_phase(new_color, phase_pattern));

            apply_window_to_buffer(
                    output_bit, window, recolored_buffer, buffer_pointer);
        }

        window_state = (window_state + 1) % window_period;

        processed = recolored_buffer[buffer_pointer];
        buffer_pointer++;
    }

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

void Knot::enable_color_manipulation(bool x){
    color_manipulation_x = x;
}

}

