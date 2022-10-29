#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#include "change/changer2.h"
#include "math/constants.h"

#include <cassert>
#include <algorithm>
#include <cmath>
#include <string.h>

#include <iostream>
#include <iomanip>

namespace change {

ChangerVars2 defaultVars2 {
    .f_freqs = 2048,
    .f_cutoff = 10000.0f,
    .f_decay = 0.01f,
    .m_decay = 0.002f,
    .f_min = 60.0f,
    .f_max = 900.0f,
    .f_voice_reco = 64,
    .f_reco = 96,
    .c_reco_rate = 2000.0f,
};

Changer2::Changer2(float rate, ChangerVars2 var){

    c_rate = rate;
    f_freqs = var.f_freqs;
    f_cutoff = std::min(var.f_cutoff, c_rate / 2);
    f_decay = std::pow(0.5f, 1.0f / (var.f_decay * c_rate));
    m_decay = std::pow(0.5f, 1.0f / (var.m_decay * c_rate));
    f_min = var.f_min;
    f_max = var.f_max;
    f_reco = var.f_reco;
    f_voice_reco = var.f_voice_reco;

    momentum.resize(f_freqs, 0.0f);
    echo.resize(f_freqs, 0.0f);
    root.resize(f_freqs, 0.0f);
    decay.resize(f_freqs, 0.0f);

    f_reso = f_cutoff / f_freqs;

    for(int i=1; i<f_freqs; i++){
        float f = std::min(0.1f, std::max(2.0f / (i * f_reso), var.f_decay));
        root[i] = std::polar(1.0, 2 * M_PI * i * f_reso / c_rate);
        decay[i] = std::pow(0.5f, 1.0f / (f * c_rate));
    }

    dbg.resize(f_freqs);

    root[0] = 1.0f;
    decay[0] = decay[1];

    f_shift = 1.0f;
    f_transform = [](std::vector<float> &freq, std::vector<float> &mag){ /* NOP */ };

    r_period = c_rate / var.c_reco_rate;
    r_slot = 0;
    r_phase = 0;

    window.resize(4 * r_period);
    for(int i=0; i<4*r_period; i++){
        window[i] = (0.5 - 0.5 * std::cos(2 * PI * i / (4 * r_period))) * 0.5;
    }

    f_pitch_estimate = 120.0f;
    c_power = 0.0f;
}

float Changer2::process(float sample){

    c_power = c_power * f_decay + (double)sample * sample * (1.0f - f_decay);

    for(int i=0; i<f_freqs; i++){
        echo[i] = echo[i] * root[i] * decay[i] + sample * (1.0f - decay[i]);
    }

    for(int i=0; i<f_freqs; i++){
        momentum[i] = momentum[i] * m_decay + (echo[i].real() * echo[i].real()
                + echo[i].imag() * echo[i].imag()) * (1.0f - m_decay);
    }

    r_phase++;
    if(r_phase >= r_period){
        r_phase = 0;
        r_slot = (r_slot + 1) % 4;
        update_reco();
    }

    float out = 0.0f;

    for(int x=0; x<4; x++){
        int i = (r_slot - x + 4) % 4;
        int z = phase[i].size();
        float s = 0.0f;
        for(int j=0; j<z; j++){
            s += (phase[i][j] * amplitude[i][j]).real();
            phase[i][j] *= speed[i][j];
        }
        out += s * window[r_period * x + r_phase];
    }
    
    return out;
}

void Changer2::update_reco(){
    
    std::vector<int> max;
    
    int N = std::ceil(f_min / f_reso);
    int cnt = N+2;
    double avg = 0;
    for(int i=0; i<N+2; i++) avg += momentum[i];

    for(int i=2; i+2<f_freqs; i++){
        if(i+N < f_freqs){ avg += momentum[i+N]; cnt++; }
        if(i-N > 0){ avg -= momentum[i-N-1]; cnt--; }
        if(     i * f_reso > f_min && 
                momentum[i] > 1.05f * avg / cnt &&
                momentum[i-2] < momentum[i] &&
                momentum[i-1] < momentum[i] &&
                momentum[i+1] < momentum[i] &&
                momentum[i+2] < momentum[i]){
            if(!max.empty()){
                int j = max.back();
                if((i-j) * f_reso < f_min){
                    if(momentum[i] > momentum[j]){
                        max.pop_back();
                        max.push_back(i);
                    }
                } else {
                    max.push_back(i);
                }
            } else {
                max.push_back(i);
            }
        }
    }

    std::sort(max.begin(), max.end(), [&](int l, int r){ return momentum[l] > momentum[r]; });
    if((int)max.size() > f_voice_reco) max.resize(f_voice_reco);
    std::sort(max.begin(), max.end());

    int z = max.size();

    std::vector<float> fmax, amp;
    for(int i : max){
        auto [f, m] = para_peak(i,
                std::sqrt(momentum[i-1]),
                std::sqrt(momentum[i]),
                std::sqrt(momentum[i+1]));
        fmax.push_back(f * f_reso);
        amp.push_back(m);
    }

    auto int_error = [&](float f){
        f = f - (int)f;
        return f > 0.5f ? 1.0f - f : f;
    };

    std::vector<float> pitch(z, 1e9f), merit(z, -1.0f);

    for(int i=0; i<z; i++){
        if(i > 0){
            float d = fmax[i] - fmax[i-1];
            float m = amp[i-1] * (0.2f - int_error(fmax[i] / d));
            if(m > 0.0f && m > merit[i] && d >= f_min && d <= f_max){
                merit[i] = m;
                pitch[i] = d;
            }
        }
        if(i+1 < z){
            float d = fmax[i+1] - fmax[i];
            float m = amp[i+1] * (0.2f - int_error(fmax[i] / d));
            if(m > 0.0f && m > merit[i]){
                merit[i] = m;
                pitch[i] = d;
            }
        }
        if(fmax[i] >= f_min && fmax[i] <= f_max && merit[i] < 0.1f * amp[i]){
            merit[i] = amp[i] * 0.5f;
            pitch[i] = fmax[i];
        }
    }

    std::vector<float> fgroup, mgroup;
    std::vector<int> order(z), group(z, -1);
    for(int i=0; i<(int)order.size(); i++) order[i] = i;
    std::sort(order.begin(), order.end(), [&](int l, int r){ return pitch[l] < pitch[r]; });

    double fsum = 0.0, fw = 0.0;
    for(int i : order){
        if(pitch[i] > f_max || merit[i] <= 0.0f) continue;
        if(pitch[i] * fw > 1.1f * fsum){
            fgroup.push_back(fsum / fw);
            mgroup.push_back(fw);
            fsum = fw = 0.0;
        }
        group[i] = fgroup.size();
        fw += merit[i];
        fsum += pitch[i] * merit[i];
    }
   
    if(fw != 0.0f && fsum != 0.0f){
        fgroup.push_back(fsum / fw);
        mgroup.push_back(fw);
    }

    int zz = fgroup.size();
    for(int i=zz-1; i>=0; i--){
        for(int j=i+1; j<zz; j++){
            if( int_error(fgroup[j]/fgroup[i]) < 0.1f &&
                mgroup[i] > mgroup[j] * 0.5f){
                mgroup[j] = 0.0f;
            }
        }
    }

    order.resize(fgroup.size());
    for(int i=0; i<(int)order.size(); i++) order[i] = i;
    std::sort(order.begin(), order.end(), [&](int l, int r){ return mgroup[l] > mgroup[r]; });

    for(int i=0; i<z; i++) pitch[i] = -1.0f;
    for(int j : order){
        for(int i=0; i<z; i++){
            if(pitch[i] != -1.0f) continue;
            if(int_error(fmax[i]/fgroup[j]) < 0.1f) pitch[i] = fgroup[j];
        }
    }

    if(fgroup.size()){
        float f = fgroup[order[0]];
        f_pitch_estimate = 0.9f * f_pitch_estimate + 0.1f * f;
    }


}

std::pair<float, float> Changer2::para_peak(int i, float l, float m, float r){
    
    // construct a parabola using the three points around
    // the peak. set the peak as origo and find the
    // bottom of the parabola.

    // y = ax^2 + bx + c

    const float a = 0.5f * (r+l) - m;
    const float b = (r-l) * 0.5f;
    const float d = - b / (2 * a);

    return {i + d, m + b*d + a*d*d};
}

void Changer2::set_pitch_shift(float shift){
    f_shift = shift;
}

void Changer2::set_transform(func f){
    f_transform = f;
}

float Changer2::get_pitch(){ return f_pitch_estimate; }

std::vector<float> Changer2::debug(){
    return dbg;
}

}

