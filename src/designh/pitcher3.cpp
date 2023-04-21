#include "designh/pitcher3.h"

#include "designh/common.h"
#include "designh/math.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

namespace designh {

// Pitcher ////////////////////////////////////////////////////////////////////

Pitcher3::Pitcher3(int framerate, float minPitchHZ){

    float low = minPitchHZ;
    float high = 1200.0f;
    float calc_frequency = 200.0f;
    float pop_length = framerate / 200.0f;

    assert(low < high);
    
    detector.pop = std::max<int>(8, std::ceil(pop_length / 2));
    detector.min = std::floor(framerate / high);
    detector.max = std::ceil(framerate / low);
    
    detector.min = std::min(detector.pop, std::max(8, detector.min));
    detector.max = std::max(2 * detector.pop, detector.max);

    detector.period = detector.pop;
    detector.similarity = 0.0;
    detector.calc_state = 0;
    detector.calc_period = std::ceil(framerate / calc_frequency);

    hpset.init("hp4");

    int max_wavelet_radius = std::ceil(hpset.max_length / 2);
    int detector_required_input_radius = detector.max + detector.pop;
    int hpset_required_input_radius = detector.max + max_wavelet_radius
        + std::ceil(hpset.max_shift);

    int input_radius = std::max(detector_required_input_radius, hpset_required_input_radius);

    ibuff.resize(2 * input_radius, 0.0f);
    obuff.resize(2 * max_wavelet_radius + 2, 0.0f);

    ibuff.set_offset(input_radius);
    obuff.set_offset(max_wavelet_radius + 1);

    pointer = 0.0f;
}

float Pitcher3::process(float sample){

    ibuff.push(sample);
    obuff.push(0.0f);

    pointer += hpset.shift - 1;

    if(detector.tick()) find_period();

    if(3 * std::abs(pointer) >= 2 * detector.period){
        float dir = (pointer > 0) - (pointer < 0);
        while(pointer * dir > 0) pointer += detector.period;
    }

    for(auto &w : hpset.wavelets){
        while(w.state <= 0.0f){
            apply_wavelet(w);
            w.state += w.length / 4;
        }
        w.increment();
    }

    return obuff[-obuff.left()] / 6;
}

void Pitcher3::set_absolute_pitch_shift(float s){

    hpset.shift = std::max(1.0f / hpset.max_shift, std::min(s, hpset.max_shift));
    
    int new_radius = std::ceil(hpset.max_length / hpset.shift / 2);
    if(new_radius + 1 > obuff.left()){
        obuff.resize(2 * new_radius + 2, 0.0f);
        obuff.set_offset(new_radius + 1);
    }
}

void Pitcher3::set_color_shift(float s){
    s = std::max(0.0f, std::min(s, 1.0f));
    // NOP
}

int Pitcher3::get_delay(){ return ibuff.right() + obuff.left(); }

void Pitcher3::find_period(){

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
    }
}

void Pitcher3::apply_wavelet(Wavelet &w){

    float radius = w.length / 2;
    float sfreq = 2 * w.spins * M_PI / w.length;
    float wfreq = 2 * M_PI / w.length;

    std::complex<float> sum = 0.0f;
    
    float input_position = pointer;
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

float Pitcher3::Wavelet::frequency_response(float framerate, float frequency){

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

void Pitcher3::Wavelet::increment(){
    state -= shift;
}

// HodgepodgeSet //////////////////////////////////////////////////////////////

void Pitcher3::HodgepodgeSet::init(std::string version){
    
    std::ifstream I("wavelets/" + version);

    std::vector<Pitcher3::Wavelet> set;

    Pitcher3::Wavelet wavelet;
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

// Detector ///////////////////////////////////////////////////////////////////

bool Pitcher3::Detector::tick(){
    calc_state = (calc_state + 1) % calc_period;
    return calc_state == 0;
}

}
