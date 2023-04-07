#include "designh/pitcher.h"

#include "designh/common.h"
#include "designh/math.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

namespace designh {

// Pitcher ////////////////////////////////////////////////////////////////////

Pitcher::Pitcher(int framerate, float minPitchHZ) :
    enveloper(64),
    delayer(64, 0.0f)
{
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
    wavelet = hodgepodge("hp3");

    int maxlen = 0;
    for(auto &i : wavelet) maxlen = std::max<int>(maxlen, i.length);

    bsize = max + std::max<int>(pop, maxlen + 1);

    ibuff.resize(max + 2 * bsize, 0.0f);
    obuff.resize(bsize, 0.0f);

    ibuff.set_offset(max + bsize);
    
    energyquery.init(max + 2 * bsize);
    energyquery.set_offset(max + bsize);

    similarity = 0.0f;
    calc_state = 0;
    calc_period = std::ceil(framerate / calc_frequency);
   
    lock_wavelets();
}

float Pitcher::process(float sample){

    delayer.push(sample);
    ibuff.push(delayer[0]);
    obuff.push(0.0f);
    
    energyquery.push(enveloper.process(sample));

    if(calc_state == 0) calc_scale();
    calc_state = (calc_state + 1) % calc_period;

    ipointer -= 1;
    fpointer += shift - 1;

    while(ipointer <= fpointer){
        
        float outer = energyquery.max(ipointer - 2 * scale / 3, ipointer + scale / 3);
        float inner = energyquery.max(ipointer - scale / 8, ipointer + scale / 8);

        if(outer == inner && no_jump > scale / 2){
            if(similarity > 0.7f && scale > 300){
                std::cerr << scale << ' ' << no_jump << ' ' << inner << ' ' << outer << '\n';
            }
            lock_wavelets();
        }

        if(3 * std::abs(ipointer) >= 2 * scale){

            double dir = (ipointer > 0) - (ipointer < 0);
            double len = 0.0;
            
            while((ipointer - len * dir) * dir > 0) len += scale;

            int move = std::round(len * dir);

            ipointer -= move;
            fpointer -= move;
        }
        
        ipointer++;
        no_jump++;
    }

    for(auto &w : wavelet){

        w.ipointer -= 1;
        w.fpointer += w.shift - 1;

        while(w.ipointer <= w.fpointer){
            
            if(3 * std::abs(w.ipointer) > 2 * scale){

                double dir = (w.ipointer > 0) - (w.ipointer < 0);
                double len = 0.0;
                
                while((w.ipointer - len * dir) * dir > 0) len += scale;

                int move = std::round(len * dir);

                w.ipointer -= move;
                w.fpointer -= move;
            }

            if(w.state <= 0){
                
                {
                    float factor = 1.0f;
                    
                    if(scale * w.spins < w.length * 3.0f){
                        float f3 = 3.0f / scale;
                        float f = w.spins / w.length;
                        factor = 0.5f - 0.5f * std::cos(M_PI * f / f3);
                    }

                    w.update_shift(factor * w.target_shift + shift * (1.0f - factor));
                }

                
                std::complex<float> sum = 0.0f;
                
                for(int i=0; i<w.length; i++) sum += ibuff[w.ipointer + i] * std::conj(w.w[i]);
                
                float phase = std::arg(sum);
                float amplitude = std::abs(sum) / w.length * w.gain;
                
                const float wrot = 2 * M_PI / w.length;
                const float frot = 2 * M_PI * w.spins / w.length;

                int i = 0;
                float j = w.fpointer - w.ipointer;

                while(j < w.length){
                    obuff[i] += amplitude * std::cos(phase + j * frot)
                        * (1.0f - std::cos(j * wrot));
                    i++;
                    j += w.shift;
                }
                
                w.state = w.length / 4;
            }

            w.state--;
            w.ipointer++;
        }
    }

    return obuff[0] / 6;
}

void Pitcher::set_absolute_pitch_shift(float s){
    shift = std::max(1.0f / 8, std::min(s, 8.0f));
    obuff.resize((int)std::ceil(bsize / shift), 0.0f);
    wavelet[0].target_shift = shift;
    wavelet[1].target_shift = shift;
}

void Pitcher::set_color_shift(float s){
    s = std::max(0.0f, std::min(s, 1.0f));
    s = s * shift + 1.0f - s;
    for(auto &w : wavelet) w.target_shift = s;
    wavelet[0].target_shift = shift;
    wavelet[1].target_shift = shift;
}

int Pitcher::get_delay(){ return bsize + enveloper.get_delay(); }

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

    similarity = mse[std::floor(scale)];
}

void Pitcher::lock_wavelets(){

    no_jump = 0;

    for(auto &w : wavelet){
        w.fpointer = fpointer;
        w.ipointer = ipointer;
    }
}

// Wavelet ////////////////////////////////////////////////////////////////////

Wavelet::Wavelet() :
    length(0),
    state(0),
    spins(0),
    gain(2),
    target_shift(1.0),
    shift(1.0)
{}

Wavelet::Wavelet(int length, float spins, float gain) : 
    length(length),
    state(0),
    spins(spins),
    gain(gain),
    w(cos_wavelet(length, spins)),
    target_shift(1.0),
    shift(1.0)
{}

std::complex<float> Wavelet::eval(float t){
    return std::complex<float>(1.0f, 2 * M_PI * t * spins / length);
}

float Wavelet::frequency_response(float framerate, float frequency){

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

void Wavelet::update_shift(float target){
    shift = std::max<float>(0.9 * shift, std::min<float>(target, shift * 1.1));
}

std::vector<Wavelet> hodgepodge(std::string version){
    
    std::ifstream I("wavelets/"+version);

    std::vector<Wavelet> set;

    int len; float freq; float gain;
    while(I >> len){
        I >> freq >> gain;
        set.emplace_back(len, freq, gain);
    }

    return set;
}

}

