#include "designa/knot.h"
#include "designa/color.h"
#include "designa/math.h"

#include <cassert>
#include <cmath>

namespace designa {

Knot::Knot(int frame_rate, float min_frequency) :
    pace(frame_rate, min_frequency, 1200.0f),
    ftip(16),
    frame_rate(frame_rate)
{
    ftip.set_shift(1.0f);
    
    pitch_shift_delay = pace.get_delay() + ftip.get_delay();
    
    absolute_pitch_shift = 1.0f;
    
    pitch_correction_x = 0;
    pitch_correction_shift = 1.0f;
    
    set_pitch_correction_frequency(440.0f);
    set_pitch_correction_scale(8);
    set_pitch_correction_power(1.0f);
    
    for(int i=0; i<pitch_shift_delay-2; i++) delay_pipe.push_back(0.0f);

    window_size = std::ceil(frame_rate / min_frequency);
    previous_window_width = window_size / 2;
    buffer_size = 4 * window_size;
    buffer_pointer = 0;
    
    window_state = 0;
    
    shifted_buffer.resize(buffer_size, 0.0f);
    original_buffer.resize(buffer_size, 0.0f);
    recolored_buffer.resize(buffer_size, 0.0f);
    wave_period.resize(buffer_size, window_size / 2);
}

float Knot::process(float sample){
  
    float processed = sample;

    {
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
        
        float raw_sample = delay_pipe.front();
        float pitch_shifted_sample = shift_pipe.front();
        
        shifted_buffer[buffer_pointer + window_size - 1] = pitch_shifted_sample + rnd(1e-6f);
        original_buffer[buffer_pointer + window_size - 1] = raw_sample + rnd(1e-6f);
        wave_period[buffer_pointer + window_size - 1] = pace.get_scale();
        
        delay_pipe.pop_front();
        shift_pipe.pop_front();
    
        processed = pitch_shifted_sample;
    }
    
    if(buffer_pointer + window_size > buffer_size){
        original_buffer = original_buffer.shift(buffer_pointer);
        shifted_buffer = shifted_buffer.shift(buffer_pointer);
        recolored_buffer = recolored_buffer.shift(buffer_pointer);
        wave_period = wave_period.shift(buffer_pointer);
        buffer_pointer = 0;
    }

    if(window_state <= 0){
        
        int right_width = wave_period[buffer_pointer];
        right_width = std::max<int>(std::floor(previous_window_width / 1.1),
                std::min<int>(right_width, std::ceil(previous_window_width * 1.1)));

        int width = previous_window_width / 2 + right_width / 2;
        assert(width <= window_size);

        auto window = cos_window(previous_window_width / 2, right_width / 2);

        int big_width = pow2ceil(width);

        auto bit = pick_bit(shifted_buffer, buffer_pointer, width);
        bit.resize(big_width, 0.0f);

        apply_window(bit, window);

        auto freq = fft(bit);
        auto energy = frequency_energies(freq);
        
        std::vector<float> shift(big_width, (1.0f / absolute_pitch_shift) * 0.5f + 0.5f);
        energy = shift_bins(energy, shift);

        auto phase = frequency_phases(freq);
        freq = join_energy_to_phase(energy, phase);

        bit = inverse_fft(freq);
        apply_window(bit, window);

        bit.resize(width);
        add_bit(bit, recolored_buffer, buffer_pointer);

        window_state = (right_width + 3) / 4;
        previous_window_width = right_width;

        processed = recolored_buffer[buffer_pointer];
    }

    window_state--;
    buffer_pointer++;

    return processed;
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

