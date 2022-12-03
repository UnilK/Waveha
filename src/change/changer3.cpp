#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#include "change/changer3.h"
#include "math/constants.h"

#include <cassert>
#include <algorithm>
#include <cmath>

#include <iostream>
#include <iomanip>

namespace change {

ChangerVars3 defaultVars3 {
    .f_freqs = 128,
    .f_min = 40.0f,
    .f_max = 10000.0f,
    .f_decay = 2.5f,
    .e_freqs = 32,
    .e_min = 32.0f,
    .e_max = 256.0f,
    .e_decay = 0.01f,
};

Changer3::Changer3(float rate){
    var = defaultVars3;
    f_shift = 1.0f;
    set_framerate(rate);
}

void Changer3::set_framerate(float rate){
    c_rate = rate;
    config(var);
}

void Changer3::config(ChangerVars3 v){
    
    var = v;
    f_freqs = v.f_freqs;
    f_min = v.f_min;
    f_max = v.f_max;
    f_decay_factor = v.f_decay;
    e_freqs = v.f_freqs;
    e_min = v.f_min;
    e_max = v.f_max;
    e_decay = std::pow(0.5, 1.0 / (c_rate * v.e_decay));

    {
        f_decay.resize(f_freqs);
        f_rota.resize(f_freqs);
        o_phase.resize(f_freqs);
        o_rota.resize(f_freqs);

        double step = std::pow((double)f_max / f_min, 1.0 / f_freqs);
        double freq = f_min;
        for(int i=0; i<f_freqs; i++){
            o_phase[i] = 0.0f;
            o_rota[i] = 2 * PI * freq / c_rate;
            f_decay[i] = std::pow(0.5, 1.0 / (c_rate / freq * f_decay_factor));
            f_rota[i] = std::polar(1.0, 2 * PI * freq / c_rate);
            freq *= step;
        }
    }
    
    {
        e_rota.resize(e_freqs);

        double step = std::pow((double)e_max / e_min, 1.0 / e_freqs);
        double freq = e_min;
        for(int i=0; i<e_freqs; i++){
            e_rota[i] = std::polar(1.0, 2 * PI * freq / c_rate);
            freq *= step;
        }
    }

    {
        f_echo.resize(f_freqs);
        for(auto &i : f_echo) i = 0.0f;

        f_envelope.resize(f_freqs, std::vector<cfloat>(e_freqs));
        for(auto &i : f_envelope) for(auto &j : i) j = 0.0f;
        
        f_amp.resize(f_freqs);
        for(auto &i : f_amp){
            i.clear();
            for(int j=0; j<2000; j++) i.push_back(0.0f);
        }
    }

}

float Changer3::process(float sample){
    
    for(int i=0; i<f_freqs; i++){
        f_echo[i] = f_echo[i] * f_rota[i] * f_decay[i] + sample * (1.0f - f_decay[i]);
        f_amp[i].pop_front();
        f_amp[i].push_back(std::abs(f_echo[i]));
    }

    float out = 0.0f;

    for(int i=0; i<f_freqs; i++){
        out += std::abs(f_echo[i]) * std::cos(o_phase[i]);
        o_phase[i] += o_rota[i];
        if(o_phase[i] > PIF) o_phase[i] -= 2 * PIF;
    }

    return out / 2.0f;
}

void Changer3::set_pitch_shift(float shift){
    f_shift = shift;
    config(var);
}

int lol = 0;
std::vector<float> Changer3::debug(){
    lol = (lol + 1) % f_freqs;
    return std::vector<float>(f_amp[lol].begin(), f_amp[lol].end());
}

}

