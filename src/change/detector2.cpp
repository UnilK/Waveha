#include "change/detector2.h"
#include "math/constants.h"

#include <algorithm>
#include <cmath>

namespace change {

Detector2::Detector2(int rate_, float low_, float high, int points){
    
    rate = rate_;
    low = low_ / 1.1;
    f0 = low_;

    size = points;
    step = (1.1 * high - low) / size;
    
    root.resize(size);
    echo.resize(size, 0.0f);
    energy.resize(size, 0.0f);
    dbg.resize(size, 0.0f);

    float decay = std::pow(0.5f, low_ / rate);
    for(int i=0; i<size; i++) root[i] = std::polar(decay, 2 * PIF * (low + i * step) / rate);

}

void Detector2::push(float sample){
    for(int i=0; i<size; i++) echo[i] = sample + echo[i] * root[i];
}

float Detector2::pitch(){

    for(int i=0; i<size; i++) energy[i] = std::norm(echo[i]);
  
    std::vector<float> avg(size, 0.0f);
    {
        int N = std::max(4, size / 16);
        double sum = 0.0;

        int cnt = N;
        for(int i=0; i<N; i++) sum += energy[i];
        for(int i=0; i<size; i++){
            if(i+N < size){ sum += energy[i+N]; cnt++; }
            if(i-N > 0){ sum -= energy[i-N-1]; cnt--; }
            avg[i] = sum / cnt;
        }
    }

    auto parabola = [](float i, float l, float m, float r) -> std::pair<float, float> {
    
        const float a = 0.5f * (r+l) - m;
        const float b = (r-l) * 0.5f;
        const float d = - b / (2 * a);

        return {i + d, m + b*d + a*d*d};
    };

    std::vector<std::pair<float, float> > peak;

    for(int i=1; i+1<size; i++){
        if(energy[i] > energy[i-1] && energy[i] > energy[i+1] && energy[i] > avg[i] * 1.25){
            auto [freq, amp] = parabola(i, energy[i-1], energy[i], energy[i+1]);
            peak.push_back({low+step*freq, amp});
        }
    }

    std::sort(peak.begin(), peak.end(), [](auto l, auto r){ return l.second > r.second; });
    peak.resize(std::min(8, (int)peak.size()));
    std::sort(peak.begin(), peak.end());

    dbg = energy;
    for(auto [f, a] : peak) dbg[std::round((f-low)/step)] *= -1;

    auto func = [&](float f){
        float sum = 0.0f, inv = 1.0f / f;
        for(auto [freq, amp] : peak){
            float d = (0.5f - std::abs(freq - f * std::round(freq * inv)) * inv);
            sum += (amp * d) * (d * d);
        }
        return sum;
    };

    if(peak.empty()) return f0;

    float best = 0.0f;
    for(auto [freq, amp] : peak){
        {
            float d = func(freq);
            if(d > best){ best = d; f0 = freq; }
        }
        {
            float d = func(freq/2);
            if(d > 1.2 * best){ best = d / 1.2; f0 = freq/2; }
        }
    }

    return f0;
}

int Detector2::period(){
    return std::round(rate / pitch());
}

std::vector<float> Detector2::debug(){
    return dbg;
}

}

