#include "change/phaser3.h"
#include "math/constants.h"
#include "math/fft.h"

#include <cmath>
#include <complex>
#include <cassert>
#include <iostream>

namespace change {

Phaser3::Phaser3(int rate, float low, float high){
    
    if(low > high) std::swap(low, high);
    
    min = std::floor(rate/high);
    max = std::ceil(rate/low);
    
    twindow = min / 2;
    twindow = std::max(2, twindow);
    min = std::max(min, 4*twindow);
    state = 0;

    size = 1;
    while(size < max) size *= 2;

    old = now = 4 * size - min;

    buffer.resize(4 * size + twindow, 0.0f);
}

void Phaser3::push(float sample){ buffer.push_back(sample); }

float Phaser3::pull(){

    if(state == 0 && now > (int)buffer.size() / 2 && now > 4 * size){
        int move = now-2*size;
        for(int i=0; i+move<(int)buffer.size(); i++) buffer[i] = buffer[i+move];
        buffer.resize(buffer.size()-move);
        now -= move;
    }

    if(now + twindow >= (int)buffer.size()){
        state = twindow;
        old = now;
        now -= period();
    }
    else if(now <= (int)buffer.size() - max - 2 * twindow){
        state = twindow;
        old = now;
        now += period();
    }

    if(state){
        float d = std::cos(PI * (state--) / (2.0 * twindow));
        return buffer[now++] * d + buffer[old++] * (1.0f - d);
    }
    return buffer[now++];
}

int Phaser3::period(){

    std::vector<float> a(max), b(max);
    for(int i=0, z=buffer.size(); i<max; i++){
        a[i] = buffer[z+i-2*max];
        b[i] = buffer[z+i-max];
    }

    auto c = math::correlation(a, b);

    int best = max;
    for(int i=min+1, j=max-min-1; i<max; i++, j--){
        if(c[j] > c[max-best] && c[j-1] < c[j] && c[j+1] < c[j]) best = i;
    }

    return best;
}

}

