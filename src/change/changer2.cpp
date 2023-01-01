#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#include "change/changer2.h"
#include "change/pitch.h"
#include "math/constants.h"
#include "math/fft.h"
#include "math/ft.h"

#include <cassert>
#include <algorithm>
#include <cmath>
#include <string.h>

#include <iostream>
#include <iomanip>

namespace change {

ChangerVars2 defaultVars2 {
    .f_freqs = 1024,
    .f_cutoff = 4096.0f,
    .f_decay = 0.01f,
    .m_decay = 0.002f,
    .f_min = 60.0f,
    .f_max = 900.0f,
    .f_reco = 64,
    .r_rate = 2000.0f,
    .r_window = 0.01f,
};

Changer2::Changer2(float rate, ChangerVars2 var){

    rate = std::min(std::max(8000.0f, rate), 48000.0f);

    c_rate = rate;
    f_freqs = var.f_freqs;
    f_cutoff = std::min(var.f_cutoff, c_rate / 2);
    f_decay = std::pow(0.5f, 1.0f / (var.f_decay * c_rate));
    m_decay = std::pow(0.5f, 1.0f / (var.m_decay * c_rate));
    f_min = var.f_min;
    f_max = var.f_max;
    f_reco = var.f_reco;

    momentum.resize(f_freqs, 0.0f);
    echo.resize(f_freqs, 0.0f);
    root.resize(f_freqs, 0.0f);

    f_reso = f_cutoff / f_freqs;
    for(int i=0; i<f_freqs; i++){
        root[i] = std::polar(1.0, 2 * PI * i * f_reso / c_rate);
    }

    dbg.resize(f_freqs);

    f_shift = 1.0f;

    r_phase = 0;
    r_period = std::ceil(c_rate / var.r_rate);

    {
        int d = std::max(2, (int)std::round(var.r_window * c_rate / r_period));
        r_size = d * r_period;
        
        r_window.resize(r_size);
        for(int i=0; i<r_size; i++){
            r_window[i] = (1.0f - std::cos(2 * PI * i / r_size)) / d;
        }
    }

    r_delay = c_rate / f_min;
    c_size = r_size + r_delay;
    
    in.init(r_delay);
    out.init(c_size);

    p_size = 1;
    while(p_size < 2 * r_delay) p_size *= 2;
    p_momentum.resize(p_size, 0.0f);

}

float Changer2::process(float sample){
   
    float nsample = 2 * sample * (1.0f - f_decay);
    for(int i=0; i<f_freqs; i++){
        echo[i] = echo[i] * root[i] * f_decay + nsample;
        momentum[i] = momentum[i] * m_decay + std::norm(echo[i]) * (1.0f - m_decay);
    }
    
    in.push(sample);
    out.push(0.0f);
    
    r_phase++;
    if(r_phase >= r_period){
        r_phase = 0;
        update_reco();
    }

    return out[0];
}

void Changer2::update_reco(){
    
    std::vector<int> max;
    
    {
        int N = std::ceil(0.7f * f_min / f_reso);
        int cnt = N+2;
        double avg = 0;
        for(int i=0; i<N+2; i++) avg += momentum[i];

        for(int i=2; i+2<f_freqs; i++){
            if(i+N < f_freqs){ avg += momentum[i+N]; cnt++; }
            if(i-N > 0){ avg -= momentum[i-N-1]; cnt--; }
            if(     i * f_reso > f_min && 
                    momentum[i] > avg / cnt &&
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
    }

    std::sort(max.begin(), max.end(), [&](int l, int r){ return momentum[l] > momentum[r]; });
    if((int)max.size() > f_reco) max.resize(f_reco);
    std::sort(max.begin(), max.end());

    int z = max.size();

    std::vector<float> fpeak, amp;

    {
        for(int i : max){
            auto [f, m] = para_peak(i,
                    std::sqrt(momentum[i-1]),
                    std::sqrt(momentum[i]),
                    std::sqrt(momentum[i+1]));
            fpeak.push_back(f * f_reso);
            amp.push_back(m);
        }
    }

    {
        float pitch = 100.0f;

        auto func = [&](float f){
            float sum = 0.0f, inv = 1.0f / f;
            for(int i=0; i<z; i++){
                float w = amp[i] * 1000.0f;
                float d = (0.5f - std::abs(fpeak[i] - f * std::round(fpeak[i] * inv)) * inv);
                sum += (w * d) * (w * d * d);
            }
            return sum;
        };

        float best = -1, step = f_min / 200;
        for(float f=f_min; f<=2*f_min; f+=step){
            float sum = func(f);
            if(sum > best){
                best = sum;
                pitch = f;
            }
        }

        for(int x=0; x<2; x++){
            float refine = pitch;
            step /= 10;
            for(int i=-10; i<=10; i++){
                float sum = func(pitch + step * i);
                if(sum > best){
                    best = sum;
                    refine = pitch + step * i;
                }
            }
            pitch = refine;
        }

        {
            float base = pitch, bbest = best;
            for(int i=2; i*base<=f_max; i++){
                float sum = func(i*base);
                if(sum * 1.05f > bbest){
                    pitch = i * base;
                    best = sum;
                }
            }
        }

        step = 0.1f;
        for(int x=0; x<3; x++){
            float refine = pitch;
            for(int i=-10; i<=10; i++){
                float sum = func(pitch + step * i);
                if(sum > best){
                    best = sum;
                    refine = pitch + step * i;
                }
            }
            pitch = refine;
            step /= 10;
        }
        
        f_pitch = std::max(f_min, std::min(f_max, pitch));
    }

    {
        int len = std::round(c_rate / f_pitch);

        std::vector<float> wave(len);
        for(int i=0; i<len; i++) wave[i] = in[i];
        smooth_clip(wave);

        {
            int F = std::round(10000.0f * len / c_rate);
            auto freq = math::ft(wave, F);
            freq = translate(freq, 0.8f);
            wave = math::ift(freq, (int)std::round(len/0.7f));
            len = wave.size();
        }

        float ssum = 0.0f;
        for(float i : wave) ssum += i*i;

        int offset = 0;
        float maxp = -1e9f;

        auto cor = math::correlation(p_momentum, wave);

        for(int i=0; i<len; i++){
            if(maxp < cor[i]){
                offset = i;
                maxp = cor[i];
            }
        }

        offset = (len - offset) % len;

        // dbg = cor;

        for(int i=0; i<p_size; i++){
            p_momentum[i] = wave[(i+offset+r_period)%len];
        }
        
        bool voiced = (maxp / ssum) > 0.7f;

        if(voiced){
            for(int i=0; i<r_size; i++){
                out[i] += r_window[i] * wave[(i+offset)%len];
            }
        } else {
            for(int i=0; i<r_size; i++){
                out[i] += r_window[i] * in[i%r_delay];
            }
        }
    }

    dbg = momentum;
    for(float &i : dbg) i = std::sqrt(i);
    for(int i : max) dbg[i] = -dbg[i];
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

void Changer2::smooth_clip(std::vector<float> &data){

    if(data.size() < 20) return;

    int n = data.size();
    int d = n * 0.1;

    std::vector<float> v, l, m, r;
    for(int i=-d+1; i<0; i++) v.push_back(data[n+i]-data[n+i-1]);
    v.push_back(data[0]-data[n-1]);
    for(int i=1; i<d; i++) v.push_back(data[i]-data[i-1]);

    int z = v.size();
    l.resize(z, 0.0f);
    m.resize(z, 0.0f);
    r.resize(z, 0.0f);

    float a = 0.33f, b = std::pow(1e-2f, 1.0f / (d-1));

    l[d-1] = r[d-1] = a;
    m[d-1] = 1.0f - 2 * a;
    for(int i=1; i<d; i++){
        r[d-1-i] = l[d-1+i] = l[d-i];
        l[d-1-i] = r[d-1+i] = b * r[d-1-i];
        m[d-1-i] = m[d-1+i] = 1.0f - l[d-i-1] - r[d-i-1];
    }
    m[0] = m[z-1] = m[0] - l[0];
    l[0] = r[z-1] = 0.0f;

    int cnt = 0;
    for(int i=0; i<10; i++){
        std::vector<float> w(z, 0.0f);
        for(int i=1; i+1<z; i++){
            w[i] = v[i] * m[i] + v[i-1] * r[i-1] + v[i+1] * l[i+1];
        }
        w[0] = v[0] * m[0] + v[1] * l[1];
        w[z-1] = v[z-1] * m[z-1] + v[z-2] * r[z-2];
        v = w;
        cnt++;
    }

    for(int i=-d+1; i<0; i++) data[n+i] = data[n+i-1] + v[d-1+i];
    data[0] = data[n-1] + v[d-1];
    for(int i=1; i<d; i++) data[i] = data[i-1] + v[d-1+i];
}

void Changer2::set_pitch_shift(float shift){
    f_shift = shift;
}

float Changer2::get_pitch(){ return f_pitch; }

std::vector<float> Changer2::debug(){
    return dbg;
}

}

