#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#include "change/detector2.h"
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

DetectorVars2 ddefaultVars2 {
    .f_freqs = 512,
    .f_cutoff = 2048.0f,
    .f_decay = 0.01f,
    .m_decay = 0.002f,
    .f_reco = 64,
};

Detector2::Detector2(int rate, float low, float high, DetectorVars2 var){

    c_rate = rate;
    f_freqs = var.f_freqs;
    f_cutoff = std::min(var.f_cutoff, c_rate / 2);
    f_decay = std::pow(0.5f, 1.0f / (var.f_decay * c_rate));
    m_decay = std::pow(0.5f, 1.0f / (var.m_decay * c_rate));
    f_min = low;
    f_max = high;
    f_reco = var.f_reco;

    momentum.resize(f_freqs, 0.0f);
    echo.resize(f_freqs, 0.0f);
    root.resize(f_freqs, 0.0f);

    f_reso = f_cutoff / f_freqs;
    for(int i=0; i<f_freqs; i++){
        root[i] = std::polar(1.0, 2 * PI * i * f_reso / c_rate);
    }

    f_pitch = 100.0f;
}

void Detector2::push(float sample){
    float nsample = 2 * sample * (1.0f - f_decay);
    for(int i=0; i<f_freqs; i++){
        echo[i] = echo[i] * root[i] * f_decay + nsample;
        momentum[i] = momentum[i] * m_decay + std::norm(echo[i]) * (1.0f - m_decay);
    }
}

float Detector2::get_pitch(){
    
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

    return f_pitch;
}

std::pair<float, float> Detector2::para_peak(int i, float l, float m, float r){
    
    // construct a parabola using the three points around
    // the peak. set the peak as origo and find the
    // bottom of the parabola.

    // y = ax^2 + bx + c

    const float a = 0.5f * (r+l) - m;
    const float b = (r-l) * 0.5f;
    const float d = - b / (2 * a);

    return {i + d, m + b*d + a*d*d};
}

std::vector<float> Detector2::debug(){
    return {0.0f, 0.0f};
}

}

