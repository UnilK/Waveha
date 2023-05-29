#include "designi/pitcher2.h"

#include "designi/common.h"
#include "designi/math.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

namespace designi {

// Pitcher2 ///////////////////////////////////////////////////////////////////

Pitcher2::Pitcher2(int framerate, float minPitchHZ) : frame_rate(framerate) {
   
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
    detector.pitch = frame_rate / detector.pop;
    detector.similarity = 0.0;
    detector.calc_state = 0;
    detector.calc_period = std::ceil(framerate / calc_frequency);
    
    segment.left = 0;
    segment.right = detector.pop;
    segment.pointer = 0.0f;
    segment.offset = 0;
    segment.shift = 1.0f;
    segment.peaks.push_back(0);
    segment.zero_offset = 0.3f;
    segment.slow_cut = 0.6f;
    segment.fast_precut = 0.4f;
    segment.fast_postcut = 0.8f;
    segment.previous_peak = 0.0f;

    hpset.init("hp4", frame_rate);

    int max_wavelet_radius = std::ceil(hpset.max_length / 2);
    int detector_required_input_radius = detector.max + detector.pop;
    int hpset_required_input_radius = detector.max + max_wavelet_radius
        + std::ceil(hpset.max_shift);

    int input_radius = std::max(detector_required_input_radius, hpset_required_input_radius);

    ibuff.resize(2 * input_radius + splitter.length, 0.0f);
    obuff.resize(2 * max_wavelet_radius + 2, 0.0f);
    ebuff.resize(2 * input_radius + splitter.length, 0.0f);

    ibuff.set_offset(input_radius);
    obuff.set_offset(max_wavelet_radius + 1);
    ebuff.set_offset(input_radius);

    update_shifts();
}

float Pitcher2::process(float sample){

    ibuff.push(sample);
    obuff.push(0.0f);
    ebuff.push(0.0f);

    if(splitter.tick()) apply_splitter();

    if(detector.tick()) find_period();

    if(segment.peaks.back() + segment.offset <= 0) add_peak();

    push_segment();

    update_pointer();

    update_segment();

    for(auto &w : hpset.wavelets){
        while(w.state <= 0.0f){
            auto input_positions = calculate_positions(w.shift);
            for(float position : input_positions) apply_wavelet(w, position);
            w.state += w.length / 4;
        }
        w.state -= w.shift;
    }

    // return ebuff[0];
    return obuff[-obuff.left()] / 6;
}

void Pitcher2::set_absolute_pitch_shift(float s){

    segment.shift = std::max(1.0f / hpset.max_shift, std::min(s, hpset.max_shift));
    
    int new_radius = std::ceil(hpset.max_length / segment.shift / 2);
    if(new_radius + 1 > obuff.left()){
        obuff.resize(2 * new_radius + 2, 0.0f);
        obuff.set_offset(new_radius + 1);
    }

    update_shifts();
}

void Pitcher2::set_color_shift(float s){
    
    float shift = std::max(1.0f / hpset.max_shift, std::min(s, hpset.max_shift));

    for(auto &w : hpset.wavelets) w.target_shift = shift;

    update_shifts();
}

int Pitcher2::get_delay(){ return ibuff.right() + obuff.left(); }

void Pitcher2::find_period(){

    std::vector<float> mse;

    {
        auto l = ibuff.slice(-detector.max-detector.pop, detector.pop);
        auto r = ibuff.slice(-detector.pop, detector.max+detector.pop);

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
        detector.pitch = frame_rate / top;
    }

    update_shifts();
}

void Pitcher2::apply_splitter(){
 
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
        ebuff[pointer + i] += splitter.window[i] * energy * splitter.energy_weight
            + splitter.lowpass_weight * normalize * (
                freq[0].real() * splitter.window[i]
                + (freq[1] * splitter.wavelet1[i]).real()
                + (freq[2] * splitter.wavelet2[i]).real());
    }
}

void Pitcher2::push_segment(){
    
    segment.left--;
    segment.right--;
    segment.offset--;
    segment.pointer += segment.shift - 1.0f;

    if(segment.offset < - 10 * frame_rate){
        for(int &i : segment.peaks) i += segment.offset;
        segment.offset = 0;
    }
}

bool Pitcher2::add_peak(){
 
    int rightmost = segment.offset + segment.peaks.back();

    if(rightmost + detector.period * 1.05 > detector.max) return 0;
    
    float factor = 2.0f - detector.similarity * 0.95f;

    int min = std::max<int>(detector.period / factor, detector.min);
    int max = std::min<int>(detector.period * factor, detector.max);
    max = std::min<int>(max, detector.max - rightmost);

    int max_index = rightmost + detector.period;
    int peak_index = rightmost + detector.period;
    float best_max = -1e9f, best_peak = -1e9f;

    for(int i=detector.min, potential=max; i<=max && potential >= 0; i++, potential--){

        if(i >= min){
            
            if(best_max < ebuff[rightmost + i]){
                max_index = rightmost + i;
                best_max = ebuff[max_index];
            }
            
            if(best_peak < ebuff[rightmost + i]
                && ebuff[rightmost + i] > ebuff[rightmost + i - 1]
                && ebuff[rightmost + i] > ebuff[rightmost + i + 1])
            {
                peak_index = rightmost + i;
                best_peak = ebuff[peak_index];
                if(best_peak > 1.5f * segment.previous_peak) potential = detector.period / 10;
            }

        } else if(ebuff[rightmost + i] > 1.5f * segment.previous_peak
                && detector.similarity < 0.8f){

            if(best_peak < ebuff[rightmost + i]
                && ebuff[rightmost + i] > ebuff[rightmost + i - 1]
                && ebuff[rightmost + i] > ebuff[rightmost + i + 1])
            {
                peak_index = rightmost + i;
                best_peak = ebuff[peak_index];
                potential = detector.period / 10;
            }
        }
    }

    if(best_peak > 0.5f * best_max){
        segment.peaks.push_back(peak_index - segment.offset);
        segment.previous_peak = best_peak;
    }
    else {
        segment.peaks.push_back(max_index - segment.offset);
        segment.previous_peak = best_max;
    }

    return 1;
}

void Pitcher2::update_pointer(){

    if(3 * std::abs(segment.pointer) > 2 * detector.period){
        float dir = (segment.pointer > 0) - (segment.pointer < 0);
        float len = 0.0;
        while((segment.pointer - len * dir) * dir > 0) len += detector.period;
        segment.pointer -= len * dir;
    }
}

void Pitcher2::update_segment(){

    if(segment.pointer < segment.left || segment.pointer > segment.right){

        while(segment.peaks.front() + segment.offset < - 3 * detector.max){
            segment.peaks.pop_front();
        }

        while(segment.peaks.back() + segment.offset < segment.pointer && add_peak());

        if(segment.peaks.back() + segment.offset < segment.pointer){

            segment.left = segment.peaks.back() + segment.offset;
            segment.right = segment.left + detector.period;

            while(segment.right < segment.pointer) segment.right += detector.period;

        } else {
            for(unsigned i=0; i+1<segment.peaks.size(); i++){
                if(segment.offset + segment.peaks[i] < segment.pointer
                        && segment.offset + segment.peaks[i+1] >= segment.pointer){
                    segment.left = segment.offset + segment.peaks[i];
                    segment.right = segment.offset + segment.peaks[i+1];
                    break;
                }
            }
        }
    }
}

void Pitcher2::update_shifts(){
    
    for(auto &w : hpset.wavelets){

        float factor = 1.0f;
        
        if(w.frequency < 2 * detector.pitch || w.spins < 2.0f){
            factor = 0.0f;
        } else if(w.frequency < 4 * detector.pitch){
            factor = 0.5f - 0.5f * std::cos(M_PI * (w.frequency / (2.0f * detector.pitch) - 1.0f));
        }

        w.shift = factor * w.target_shift + segment.shift * (1.0f - factor);
    }
}

std::vector<float> Pitcher2::calculate_positions(float shift){

    std::vector<float> positions;

    float state = (segment.pointer - segment.left) / (segment.right - segment.left);

    float relative_shift = shift / segment.shift;

    if(relative_shift <= 1.0f){
      
        if(state < segment.slow_cut){
            positions.push_back(state * relative_shift);
        }
        if(state >= segment.slow_cut){
            positions.push_back(1.0f - (1.0f - state) * relative_shift);
        }

    } else {

        float head = state * relative_shift;
        float tail = (1.0f - state) * relative_shift;

        float len = segment.fast_postcut - segment.fast_precut;

        while(head > segment.fast_postcut) head -= len;
        if(head + tail < 1.0f) head = 1.0f - tail;

        positions.push_back(head);
    }

    for(float &pos : positions){
        pos = segment.left + pos * (segment.right - segment.left);
        while(pos > detector.max) pos -= detector.period;
        while(pos < -detector.max) pos += detector.period;
    }

    return positions;
}


void Pitcher2::apply_wavelet(Wavelet &w, float input_position){

    float radius = w.length / 2;
    float sfreq = 2 * w.spins * M_PI / w.length;
    float wfreq = 2 * M_PI / w.length;
   
    auto input_correlation = [&](float wavelet_position){
        
        std::complex<float> sum = 0.0f;

        int input_left = std::ceil(wavelet_position - radius);
        int input_right = std::floor(wavelet_position + radius);

        input_left = std::max(-ibuff.left(), input_left);
        input_right = std::min(input_right, ibuff.right()-1);

        std::complex<float> srot = std::polar(1.0f, -sfreq);
        std::complex<float> sp = std::polar(1.0f, -sfreq *
                (input_left - wavelet_position + radius));
        
        std::complex<float> wrot = std::polar(1.0f, wfreq);
        std::complex<float> wp = std::polar(1.0f, wfreq *
                (input_left - wavelet_position + radius));

        for(int i=input_left; i<=input_right; i++){
            sum += ibuff[i] * (1.0f - wp.real()) * sp;
            sp *= srot;
            wp *= wrot;
        }
        
        sum *= w.gain / w.length;

        return sum;
    };

    // calculate the phase of the wavelet.
    auto sum = input_correlation(input_position + w.state);
    
    // calculate the amplitude of the wavelet.
    auto asum = input_correlation(segment.pointer);

    sum /= (std::abs(sum) + 1e-18f);
    sum *= std::abs(asum);

    {
        // add resampled wavelet to output
        
        int output_left = -std::floor((radius+w.state)/w.shift);
        int output_right = std::ceil((radius-w.state)/w.shift);

        output_left = std::max(-obuff.left(), output_left);
        output_right = std::min(output_right, obuff.right()-1);

        std::complex<float> srot = std::polar(1.0f, sfreq * w.shift);
        std::complex<float> sp = std::polar(1.0f, sfreq * w.shift *
                (output_left * w.shift - w.state + radius));
        
        std::complex<float> wrot = std::polar(1.0f, wfreq * w.shift);
        std::complex<float> wp = std::polar(1.0f, wfreq * w.shift *
                (output_left * w.shift - w.state + radius));

        for(int i=output_left; i<=output_right; i++){
            obuff[i] += (1.0f - wp.real()) * (sum * sp).real();
            sp *= srot;
            wp *= wrot;
        }
    }
}

// Wavelet ////////////////////////////////////////////////////////////////////

float Pitcher2::Wavelet::frequency_response(float framerate, float frequency){

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

// HodgepodgeSet //////////////////////////////////////////////////////////////

void Pitcher2::HodgepodgeSet::init(std::string version, float frame_rate){
    
    std::ifstream I("wavelets/" + version);

    std::vector<Pitcher2::Wavelet> set;

    Pitcher2::Wavelet wavelet;
    wavelet.length = 0.0f;
    wavelet.state = 0.0f;
    wavelet.spins = 0.0f;
    wavelet.gain = 0.0f;
    wavelet.target_shift = wavelet.shift = 1.0f;

    max_length = 0;
    while(I >> wavelet.length){
        I >> wavelet.spins >> wavelet.gain;
        wavelet.frequency = wavelet.spins * frame_rate / wavelet.length;
        wavelets.emplace_back(wavelet);
        max_length = std::max(max_length, wavelet.length);
    }
}

// Detector ///////////////////////////////////////////////////////////////////

bool Pitcher2::Detector::tick(){
    calc_state = (calc_state + 1) % calc_period;
    return calc_state == 0;
}

// Splitter ///////////////////////////////////////////////////////////////////

bool Pitcher2::Splitter::tick(){
    state++;
    if(state >= tick_period) state = 0;
    return state == 0;
}

}
