#include "designb/wavelet.h"
#include "designb/common.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace designb {

Wavelet::Wavelet(double length, double spins){

    wsize = length;
    spin = spins;

    win = 0;
    out = 0;
    
    shift = 1.0;
    cshift = 1.0;

    in = 0;
    iwsize = std::ceil(wsize);
    size = 4 * iwsize;

    phase = 1.0f;
    decay = 0.0f;
    energy = 0.0f;

    buffer.resize(size, 0.0f);
}

std::vector<float> Wavelet::process(float sample){
    
    if(in + iwsize > size){

        buffer = buffer.shift(in);
        for(auto &[x, y] : wavelets) x -= in;
        
        win -= in;
        out -= in;
        in = 0;
    }

    buffer[in + iwsize - 1] = sample;

    if(win <= in){
        
        std::complex<float> sum = 0.0f;

        const double a = 2 * M_PI / wsize;
        const double b = a * spin;
        
        for(int i=in; i-win < wsize; i++){
            sum += eval(a * (i - win), - b * (i - win)) * buffer[i];
        }

        sum /= (float)wsize;

        {
            const float c = a * spin * (cshift + (1.0 - cshift) / shift);
            auto inter = phase * std::polar<float>(1.0f, (c - b) * wsize / 4);

            float e = std::norm(sum);
            float f = std::max(0.0f, e - energy) / std::max(1e-18f, e + energy);

            phase = std::polar<float>(1.0f, (1.0f - f) * std::arg(inter));

            sum *= phase;
            
            energy = e * (1.0 - decay) + energy * decay;

        }

        wavelets.push_back({win, sum});

        win += wsize / 4;
    }

    std::vector<float> result;

    while(out <= in){

        while(!wavelets.empty()){
            auto [x, y] = wavelets.front();
            if(out-x >= wsize) wavelets.pop_front();
            else break;
        }
        
        const double a = 2 * M_PI / wsize;
        const double b = a * spin * (cshift + (1.0 - cshift) / shift);

        float sum = 0.0f;

        for(auto [x, y] : wavelets){
            if(x > out) break;
            sum += (eval(a * (out - x), b * (out - x)) * y).real();
        }

        result.push_back(sum / std::sqrt(3));

        out += shift;
    }

    in++;

    return result;
}

void Wavelet::set_shift(double s){
    s = std::max(0.1, std::min(s, 10.0));
    shift = s;
}

void Wavelet::set_color_shift(double s){
    s = std::max(0.0, std::min(s, 1.0));
    cshift = s;
}

void Wavelet::set_decay(float d){
    d = std::max(0.0f, std::min(d, 1.0f - 1e-6f));
    decay = d;
}

int Wavelet::get_delay(){
    return iwsize;
}

std::complex<float> Wavelet::eval(float x, float y){
    return std::polar<float>(1.0f - std::cos(x), y);
}

}

