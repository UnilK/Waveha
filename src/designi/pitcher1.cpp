#include "designi/pitcher1.h"

#include "designi/common.h"
#include "designi/math.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

namespace designi {

// Pitcher1 ///////////////////////////////////////////////////////////////////

Pitcher1::Pitcher1(int framerate, float minPitchHZ){
   
    std::cerr << "###############\n";

    float low = minPitchHZ;
    float high = 1200.0f;
    float calc_frequency = 200.0f;
    float pop_length = framerate / 200.0f;

    assert(low < high);
    
    splitter.state = 0;
    splitter.length = 64;
    splitter.window = cos_window(splitter.length);
    splitter.wavelet1 = cos_wavelet(splitter.length, 1);
    splitter.wavelet2 = cos_wavelet(splitter.length, 2);
    splitter.bit.resize(splitter.length);
    splitter.tick_period = splitter.length / 8;
    splitter.energy_weight = 4.0f;
    splitter.lowpass_weight = 1.0f;

    detector.pop = std::max<int>(8, std::ceil(pop_length / 2));
    detector.min = std::floor(framerate / high);
    detector.max = std::ceil(framerate / low);
    
    detector.min = std::min(detector.pop, std::max(8, detector.min));
    detector.max = std::max(2 * detector.pop, detector.max);

    detector.period = detector.pop;
    detector.similarity = 0.0;
    detector.calc_state = 0;
    detector.calc_period = std::ceil(framerate / calc_frequency);
    
    detector.momentum = 0.0f;

    segment.length = detector.pop;
    segment.left = 0;
    segment.right = segment.length - 1;
    segment.state = 0.0f;
    segment.slow_cut = 0.7f;
    segment.fast_precut = 0.4f;
    segment.fast_postcut = 0.8f;
    segment.previous_peak = 0.0f;

    hpset.init("hp4");

    int max_wavelet_radius = std::ceil(hpset.max_length / 2);
    int detector_required_input_radius = detector.max + detector.pop;
    int hpset_required_input_radius = detector.max + max_wavelet_radius
        + std::ceil(hpset.max_shift);

    int input_radius = std::max(detector_required_input_radius, hpset_required_input_radius);

    ibuff.resize(2 * input_radius + splitter.length, 0.0f);
    obuff.resize(2 * max_wavelet_radius + 2, 0.0f);
    ebuff.resize(2 * input_radius + splitter.length, 0.0f);
    lbuff.resize(2 * input_radius + splitter.length, 0.0f);

    ibuff.set_offset(input_radius);
    obuff.set_offset(max_wavelet_radius + 1);
    ebuff.set_offset(input_radius);
    lbuff.set_offset(input_radius);
}

float Pitcher1::process(float sample){

    ibuff.push(sample);
    obuff.push(0.0f);
    ebuff.push(0.0f);
    lbuff.push(0.0f);

    segment.push_left();
    segment.update_state(hpset.shift);

    if(splitter.tick()) apply_splitter();

    if(detector.tick()) find_period();

    if(segment.right <= 0) move_segment();

    for(auto &w : hpset.wavelets){
        while(w.state <= 0.0f){
            apply_wavelet(w);
            w.state += w.length / 4;
        }
        w.increment();
    }

    // return ebuff[0];
    // return lbuff[0];
    return obuff[-obuff.left()] / 6;
}

void Pitcher1::set_absolute_pitch_shift(float s){

    hpset.shift = std::max(1.0f / hpset.max_shift, std::min(s, hpset.max_shift));
    
    int new_radius = std::ceil(hpset.max_length / hpset.shift / 2);
    if(new_radius + 1 > obuff.left()){
        obuff.resize(2 * new_radius + 2, 0.0f);
        obuff.set_offset(new_radius + 1);
    }
}

void Pitcher1::set_color_shift(float s){
    s = std::max(0.0f, std::min(s, 1.0f));
}

int Pitcher1::get_delay(){ return ibuff.right() + obuff.left(); }

void Pitcher1::find_period(){

    std::vector<float> mse;

    {
        auto l = lbuff.slice(-detector.max-detector.pop, detector.pop);
        auto r = lbuff.slice(-detector.pop, detector.max+detector.pop);

        for(int i=0; i < detector.max + 2 * detector.pop; i++){
            const float noise = rnd(1e-4f);
            l[i] += noise;
            r[i] += noise;
        }

        mse = padded_energy_mse(l, r, detector.pop);
        for(float &i : mse) i = 1.0f - i;
    }

    {
        int top = detector.pop;

        for(int i=detector.min+1; i+1<=detector.max; i++){
            if(mse[i] > mse[top] && mse[i] > mse[i-1] && mse[i] > mse[i+1]){
                top = i;
            }
        }

        detector.similarity = std::max(0.0f, std::min(mse[top], 1.0f));
        detector.period = top;
    }
}

void Pitcher1::apply_splitter(){
 
    int pointer = ibuff.right() - splitter.length;

    for(int i=0; i<splitter.length; i++){
        splitter.bit[i] = splitter.window[i] * ibuff[pointer + i];
    }

    auto freq = fft(splitter.bit);

    float energy = 0.0f;
    for(int i=2; i<=splitter.length/2; i++) energy += std::norm(freq[i]) * i * i;

    freq[1] *= 2.0f;
    freq[2] *= 2.0f;

    const float normalize = 1.0f / (12.0f * splitter.length);
    energy *= normalize;

    for(int i=0; i<splitter.length; i++){
        ebuff[pointer + i] += splitter.window[i] * energy;
        lbuff[pointer + i] += normalize * (
                freq[0].real() * splitter.window[i]
                + (freq[1] * splitter.wavelet1[i]).real()
                + (freq[2] * splitter.wavelet2[i]).real());
    }
    
    for(int i=0; i<splitter.tick_period; i++){
        ebuff[pointer + i] = ebuff[pointer + i] * splitter.energy_weight
            + lbuff[pointer + i] * splitter.lowpass_weight;
    }
}

void Pitcher1::move_segment(){
   
    float factor = 2.0f - detector.similarity * 0.8f;

    int min = std::max<int>(detector.period / factor, detector.min);
    int max = std::min<int>(detector.period * factor, detector.max);

    int max_index = segment.right + detector.period;
    int peak_index = segment.right + detector.period;
    float best_max = -1e9f, best_peak = -1e9f;

    for(int i=detector.min, potential=max; i<=max && potential >= 0; i++, potential--){
        
        if(i >= min){
            
            if(best_max < ebuff[segment.right + i]){
                max_index = segment.right + i;
                best_max = ebuff[max_index];
            }
            
            if(best_peak < ebuff[segment.right + i]
                && ebuff[segment.right + i] > ebuff[segment.right + i - 1]
                && ebuff[segment.right + i] > ebuff[segment.right + i + 1])
            {
                peak_index = segment.right + i;
                best_peak = ebuff[peak_index];
                if(best_peak > 1.5f * segment.previous_peak) potential = detector.period / 10;
            }

        } else if(ebuff[segment.right + i] > 1.5f * segment.previous_peak
                && detector.similarity < 0.8f){

            if(best_peak < ebuff[segment.right + i]
                && ebuff[segment.right + i] > ebuff[segment.right + i - 1]
                && ebuff[segment.right + i] > ebuff[segment.right + i + 1])
            {
                peak_index = segment.right + i;
                best_peak = ebuff[peak_index];
                potential = detector.period / 10;
            }
        }

    }

    segment.left = segment.right;

    if(best_peak > 0.5f * best_max){
        segment.right = peak_index;
        segment.previous_peak = best_peak;
    }
    else {
        segment.right = max_index;
        segment.previous_peak = best_max;
    }

    // ebuff[segment.right] = -1;

    segment.length = segment.right - segment.left + 1;
}

void Pitcher1::apply_wavelet(Wavelet &w){

    float radius = w.length / 2;
    float sfreq = 2 * w.spins * M_PI / w.length;
    float wfreq = 2 * M_PI / w.length;

    std::complex<float> sum = 0.0f;
    
    float input_position = segment.calculate_position(w.shift / hpset.shift);
    float wavelet_position = input_position + w.state;

    {
        // calculate correlation from input

        int input_left = std::floor(wavelet_position);
        while(wavelet_position - input_left + 1 < radius) input_left--;
        
        int input_right = input_left;
        while(input_right + 1 - wavelet_position < radius) input_right++;

        assert(input_left >= -ibuff.left());
        assert(input_right < ibuff.right());
        input_left = std::max(-ibuff.left(), input_left);
        input_right = std::min(input_right, ibuff.right()-1);

        for(int i=input_left; i<=input_right; i++){
            const float t = i - wavelet_position + radius;
            sum += ibuff[i] * std::polar<float>(1.0f - std::cos(wfreq * t), -sfreq * t);
        }
        
        sum /= w.length;
    }

    float amplitude = std::abs(sum) * w.gain;
    float phase = std::arg(sum);

    /*
    float factor = 1.0f - detector.similarity;
    amplitude *= 1.0f + rnd(factor);
    phase += M_PI * rnd(factor);
    */

    {
        // add resampled wavelet to output
        
        int output_left = 0;
        float pointer = input_position;
        while(wavelet_position - pointer + w.shift < radius){
            output_left--;
            pointer -= w.shift;
        }
        
        int output_right = output_left;
        while(pointer - wavelet_position + w.shift < radius){
            output_right++;
            pointer += w.shift;
        }

        assert(output_left >= -obuff.left());
        assert(output_right < obuff.right());
        output_left = std::max(-obuff.left(), output_left);
        output_right = std::min(output_right, obuff.right()-1);

        pointer = input_position + output_left * w.shift;
        for(int i=output_left; i<=output_right; i++){
            const float t = pointer - wavelet_position + radius;
            obuff[i] += amplitude * (1.0f - std::cos(wfreq * t)) * std::cos(phase + sfreq * t);
            pointer += w.shift;
        }
    }
}


// Wavelet ////////////////////////////////////////////////////////////////////

float Pitcher1::Wavelet::frequency_response(float framerate, float frequency){

    auto sinc = [](float x){
        if(std::abs(x) < 1e-5f) return 1.0f;
        return std::sin(x * (float)M_PI) / (x * (float)M_PI);
    };

    float f0 = framerate / length;
    float fs = f0 * spins;
    
    auto response = [&](float f){
        return 0.25f * sinc((frequency - (f - f0)) / f0)
            + 0.5f * sinc((frequency - f) / f0)
            + 0.25f * sinc((frequency - (f + f0)) / f0);
    };

    float r = response(fs);
    if(frequency > framerate / 4) r += response(framerate - fs);
    else r += response(-fs);

    return r * gain;
}

void Pitcher1::Wavelet::increment(){
    state -= shift;
}

// HodgepodgeSet //////////////////////////////////////////////////////////////

void Pitcher1::HodgepodgeSet::init(std::string version){
    
    std::ifstream I("wavelets/" + version);

    std::vector<Pitcher1::Wavelet> set;

    Pitcher1::Wavelet wavelet;
    wavelet.length = 0.0f;
    wavelet.state = 0.0f;
    wavelet.spins = 0.0f;
    wavelet.gain = 0.0f;
    wavelet.shift = 1.0f;

    max_length = 0;
    while(I >> wavelet.length){
        I >> wavelet.spins >> wavelet.gain;
        wavelets.emplace_back(wavelet);
        max_length = std::max(max_length, wavelet.length);
    }

    shift = 1.0f;
}

// PulseSegment ///////////////////////////////////////////////////////////////

void Pitcher1::PulseSegment::push_left(){
    left--;
    right--;
}

void Pitcher1::PulseSegment::update_state(float shift){
    state += shift / length;
    while(state > 1.0f) state -= 1.0f;
}

float Pitcher1::PulseSegment::calculate_position(float relative_shift){

    if(relative_shift <= 1.0f){
        
        if(state < slow_cut) return left + length * state * relative_shift;
        return right - length * (1.0f - state) * relative_shift;

    } else {

        float head = state * relative_shift;
        float tail = (1.0f - state) * relative_shift;

        float len = fast_postcut - fast_precut;

        while(head > fast_postcut) head -= len;
        if(head + tail < 1.0f) head = 1.0f - tail;

        return left + length * head;
    }
}

// Detector ///////////////////////////////////////////////////////////////////

bool Pitcher1::Detector::tick(){
    calc_state = (calc_state + 1) % calc_period;
    return calc_state == 0;
}

// Splitter ///////////////////////////////////////////////////////////////////

bool Pitcher1::Splitter::tick(){
    state++;
    if(state >= tick_period) state = 0;
    return state == 0;
}

}

