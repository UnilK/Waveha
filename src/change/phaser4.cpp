#include "change/phaser4.h"
#include "math/fft.h"

#include <cmath>
#include <complex>
#include <cassert>
#include <iostream>

namespace change {

Phaser4::Phaser4(int rate, float low, float high){
    
    if(low > high) std::swap(low, high);

    min = std::floor(rate/high);
    max = std::ceil(rate/low);

    size = max;

    right = 3 * size - min;
    left = 3 * size - max;
    old = 1;

    decay = std::pow(1e-5, 1.0 / max);
    wl = wr = wo = 0.0f;

    buffer.resize(4 * size + 32, 0.0f);
}

void Phaser4::push(float sample){ buffer.push_back(sample); }

float Phaser4::pull(){
    
    if(left > (int)buffer.size() / 2 && left > 3 * max){
        int move = left - 2 * max;
        for(int i=0; i+move<(int)buffer.size(); i++) buffer[i] = buffer[i+move];
        buffer.resize(buffer.size() - move);
        left -= move;
        right -= move;
    }

    while(right + size >= (int)buffer.size()){
        old = 2;
        right = left;
        left -= period(left);
        std::swap(wl, wr);
    }
    while((int)buffer.size()-right > 2 * size + 32){
        old = 1;
        left = right;
        right += period(right);
        std::swap(wl, wr);
    }

    if(old == 1){
        wl = wl * decay;
        wr = wr * decay + (1.0f - decay);
    } else {
        wr = wr * decay;
        wl = wl * decay + (1.0f - decay);
    }

    float vl = wl * wl;
    float vr = wr * wr;
   
    return (buffer[left++] * vl + buffer[right++] * vr) / (vl + vr + 1e-9);
}

int Phaser4::period(int mid){

    std::vector<float> a(max), b(max);
    for(int i=0; i<max; i++){
        a[i] = buffer[mid+i-max];
        b[i] = buffer[mid+i];
    }

    auto c = math::emse(a, b);

    int best = min; float top = 1e9;
    for(int i=min+1; i+1<=max; i++){
        if(c[i] < top && c[i] < c[i-1] && c[i] < c[i+1]){ best = i; top = c[i]; }
    }

    return best;
}

}

