#include "change/phaser.h"
#include "math/fft.h"
#include "math/constants.h"

#include <cmath>
#include <complex>
#include <cassert>
#include <iostream>

namespace change {

Phaser::Phaser(int rate, float low, float high){
    
    if(low > high) std::swap(low, high);

    min = std::floor(rate/high);
    max = std::ceil(rate/low);

    size = 1;
    while(size < max) size *= 2;

    right = 4 * size - min;
    left = 4 * size - max - min;
    dist = right - left;
    out = 0;
    state = max;

    ibuff.resize(4 * size, 0.0f);
    obuff.resize(4 * size, 0.0f);
    inv.resize(max+1, 0.0f);

    for(int i=1; i<=max; i++) inv[i] = 1.0f / std::pow(i, 0.5);

    window.resize(dist, 0.0f);
}

void Phaser::push(float sample){ ibuff.push_back(sample); }

float Phaser::pull(){
    
    if(left > 4 * size && left > (int)ibuff.size() / 2){
        int move = left - 2 * max;
        for(int i=0; i+move<(int)ibuff.size(); i++) ibuff[i] = ibuff[i+move];
        ibuff.resize(ibuff.size() - move);
        left -= move;
        right -= move;
    }

    if(out + dist > 4 * size){
        for(int i=0; i<4*size; i++){
            if(i+out < 4*size) obuff[i] = obuff[i+out];
            else obuff[i] = 0.0f;
        }
        out = 0;
    }

    if(right == (int)ibuff.size()){
        right = left;
        left -= period();
    } else if((int)ibuff.size()-right > max + 32){
        left = right;
        right += period();
    }

    if(state >= dist / 4){
        for(int i=0; i<dist; i++) obuff[out+i] += window[i] * ibuff[left+i];
        state = 0;
    } else state++;

    left++; right++;
    return obuff[out++];
}

int Phaser::period(){

    std::vector<float> a(max), b(max);
    for(int i=0; i<max; i++){
        a[i] = ibuff[right+i-2*max];
        b[i] = ibuff[right+i-max];
    }

    auto c = math::correlation(a, b);

    int best = -1; float top = -1e9;
    // for(int i=min; i<=max; i++) c[max-i] *= inv[i];
    for(int i=min+1, j=max-min-1; i+1<=max; i++, j--){
        if(c[j] > top && c[j] > c[j-1] && c[j] > c[j+1]){ best = i; top = c[j]; }
    } if(best == -1) best = max;

    dist = best;
    window.resize(dist);
    for(int i=0; i<dist; i++) window[i] = (1.0f - std::cos(2 * PIF * i / dist));
    for(float &i : window) i /= 4;

    return dist;
}

}

