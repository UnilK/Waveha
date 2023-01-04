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

    size = 1;
    while(size < max) size *= 2;

    right = 4 * size - min;
    left = 4 * size - max;
    old = 4 * size - max;

    decay = std::pow(1e-9, 1.0 / size);
    wl = wr = wo = 0.0f;

    buffer.resize(4 * size + 32, 0.0f);
    inv.resize(max+1, 0.0f);

    for(int i=1; i<=max; i++) inv[i] = 1.0f / i;
}

void Phaser4::push(float sample){ buffer.push_back(sample); }

float Phaser4::pull(){
    
    if(left > (int)buffer.size() / 2
        && old > (int)buffer.size() / 2
        && 8 * size < (int)buffer.size()
        && left > 3 * max
        && old > 3 * max){
        
        int move = std::min(left, old) - 2 * max;
        for(int i=0; i+move<(int)buffer.size(); i++) buffer[i] = buffer[i+move];

        buffer.resize(buffer.size() - move);

        left -= move;
        right -= move;
        old -= move;
    }

    if(right == (int)buffer.size()){
        
        old = left; wo = 0.0f;
        right = left; wr = wl;
        left -= period(); wl = 0.0f;

    } else if((int)buffer.size()-right > size + 32){

        old = left; wo = wl;
        left = right; wl = wr;
        right += period(); wr = 0.0f;
    
    }

    wl = wl * decay + (1.0f - decay);
    wr = wr * decay + (1.0f - decay);
    wo = wo * decay;

    int z = buffer.size();
    float vl = wl * std::min(z - left, std::max(0, left-(z-2*max)));
    float vr = wr * std::min(z - right, std::max(0, right-(z-2*max)));
    float vo = wo * std::min(z - old, std::max(0, old-(z-2*max)));

    vl *= vl * vl;
    vr *= vr * vr;
    vo *= vo * vo;
    
    return (buffer[left++] * vl + buffer[right++] * vr + buffer[old++] * vo) / (vl + vr + vo + 1e-9);
}

int Phaser4::period(){

    std::vector<float> a(max), b(max);
    for(int i=0, z=buffer.size(); i<max; i++){
        a[i] = buffer[z+i-2*max];
        b[i] = buffer[z+i-max];
    }

    auto c = math::nmse(a, b);

    int best = -1; float top = 1e9;
    for(int i=min+1, j=max-min-1; i+1<=max; i++, j--){
        if(c[j] < top && c[j] < c[j-1] && c[j] < c[j+1]){ best = i; top = c[j]; }
    } if(best == -1) best = max;

    return best;
}

}

