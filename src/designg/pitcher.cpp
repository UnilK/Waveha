#include "designg/pitcher.h"

#include "designg/common.h"
#include "designg/math.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

namespace designg {

Pitcher::Pitcher(int framerate, float minPitchHZ){
    
    float low = minPitchHZ;
    float high = 1200.0f;
    float calc_frequency = 200.0f;
    float pop_length = framerate / 200.0f;

    assert(low < high);

    pop = std::max<int>(8, std::ceil(pop_length / 2));
    min = std::floor(framerate / high);
    max = std::ceil(framerate / low);

    min = std::min(pop, std::max(8, min));
    max = std::max(2 * pop, max);

    shift = 1.0;
    scale = pop;

    fpointer = 0.0;
    ipointer = 0;
    wavelet = hpset("hp3");
    shifts.resize(wavelet.size(), 1.0f);

    int maxlen = 0;
    for(auto &i : wavelet) maxlen = std::max<int>(maxlen, i.length);

    bsize = std::max<int>(max + pop, 2 * max / 3 + maxlen + 1);

    ibuff.resize(2 * bsize, 0.0f);
    obuff.resize(bsize, 0.0f);
    ibuff.set_offset(bsize);

    calc_state = 0;
    calc_period = std::ceil(framerate / calc_frequency);
}

float Pitcher::process(float sample){
    
    ibuff.push(sample);
    obuff.push(0.0f);

    if(calc_state == 0) calc_scale();
    calc_state = (calc_state + 1) % calc_period;

    ipointer -= 1;
    fpointer += shift - 1;

    while(ipointer <= fpointer){
        
        if(3 * std::abs(ipointer) > 2 * scale){

            double dir = (ipointer > 0) - (ipointer < 0);
            double len = 0.0;
            
            while((ipointer - len * dir) * dir > 0) len += scale;

            ipointer -= std::round(len * dir);
            fpointer -= std::round(len * dir);
        }

        for(int ww=0; ww<(int)wavelet.size(); ww++){

            auto &w = wavelet[ww];
            
            if(w.state <= 0){
                
                std::complex<float> sum = 0.0f;
                
                for(int i=0; i<w.length; i++) sum += ibuff[ipointer + i] * std::conj(w.w[i]);
                
                float phase = std::arg(sum);
                float amplitude = std::abs(sum) / w.length * w.gain;
                
                w.phase += 2 * M_PI * w.spins / w.length / 4 * (shifts[ww]);
                if(w.phase > 2*M_PI) w.phase -= 2 * M_PI;
                if(w.phase < -2*M_PI) w.phase += 2 * M_PI;

                /*
                if(amplitude > w.amplitude){
                    if(!w.increasing) w.phase = 0.0f;
                    w.increasing = 1;
                } else {
                    w.increasing = 0;
                }
                */

                // phase += w.phase;
                w.amplitude = amplitude;

                float wrot = 2 * M_PI / w.length;
                float frot = 2 * M_PI * w.spins / w.length;

                int i = 0;
                float j = fpointer - ipointer;

                while(j < w.length){
                    obuff[i] += amplitude * std::cos(phase + j * frot /* * shifts[ww] */)
                        * (1.0f - std::cos(j * wrot));
                    i++;
                    j += shift;
                }
                
                w.state = w.length / 4;
            }
            w.state--;
        }

        ipointer++;
    }

    return obuff[0] / 6;
}

void Pitcher::set_absolute_pitch_shift(float s){
    shift = std::max(1.0f / 8, std::min(s, 8.0f));
    obuff.resize((int)std::ceil(bsize / shift), 0.0f);
}

void Pitcher::set_color_shift(const std::vector<float> &new_shifts){
    assert(new_shifts.size() == shifts.size());
    shifts = new_shifts;
}

int Pitcher::get_shift_size(){
    return shifts.size();
}

int Pitcher::get_delay(){ return bsize; }

void Pitcher::calc_scale(){

    auto l = ibuff.slice(-max-pop, pop);
    auto r = ibuff.slice(-pop, max+pop);

    for(int i=0; i<max+2*pop; i++){
        const float noise = rnd(1e-4f);
        l[i] += noise;
        r[i] += noise;
    }

    auto mse = padded_energy_mse(l, r, pop);
    for(float &i : mse) i = 1.0f - i;
    
    scale = maximum_peak(mse, min, max);
}

}

