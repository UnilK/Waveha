#include "change/phaser2.h"
#include "math/fft.h"
#include "change/util.h"

#include <cmath>
#include <complex>
#include <cassert>
#include <iostream>

namespace change {

Phaser2::Phaser2(int rate, float low, float high, float vpop, float srek){
    
    if(low > high) std::swap(low, high);

    pop = std::max<int>(8, std::ceil(rate / (2 * vpop)));
    min = std::floor(rate/high);
    max = std::ceil(rate/low);

    min = std::min(pop, min);
    max = std::max(2 * pop, max);

    size = 4 * max + 2 * pop;
    buffer.resize(size, 0.0f);
   
    mid = max + pop;
    left = mid - min;
    right = left + pop;
    old = 1;

    scale = pop;
    similarity = 0.0f;

    calc_state = 0;
    calc_rate = std::ceil(rate / srek);

    decay = 0.5f;
    wl = wr = 0.0f;
}

void Phaser2::push(float sample){
    
    if(mid + max + pop > size){
        int move = mid - max - pop;
        for(int i=0; i+move<size; i++) buffer[i] = buffer[i+move];
        mid -= move;
        left -= move;
        right -= move;
    }

    buffer[mid+max+pop-1] = sample;

    if(calc_state == 0) calc_scale();
    calc_state = (calc_state + 1) % calc_rate;

    mid++;
}

float Phaser2::pull(){

    if(mid-left >= max+pop || right-mid >= max+pop){
        left = mid - min;
        right = left + pop;
        wl = wr = 0.0f;
    }

    if(left > mid){
        old = 1;
        right = left;
        left = match(left, left-scale);
        decay = std::pow(0.01f, 1.0f / scale);
        std::swap(wl, wr);
    }

    if(right < mid){
        old = 2;
        left = right;
        right = match(right, right+scale);
        decay = std::pow(0.01f, 1.0f / scale);
        std::swap(wl, wr);
    }

    if(old == 1){
        wl = wl * decay + 1.0f - decay;
        wr = wr * decay;
    } else {
        wr = wr * decay + 1.0f - decay;
        wl = wl * decay;
    }

    return buffer[left++] * wl + buffer[right++] * wr;
}

int Phaser2::get_scale(){ return scale; }

float Phaser2::get_similarity(){ return similarity; }

int Phaser2::get_delay(){ return max + pop; }

void Phaser2::calc_scale(){

    std::vector<float> l(max+2*pop), r(max+2*pop);
    for(int i=0; i<max+2*pop; i++){
        l[i] = buffer[mid+i-max-pop] + rnd() * 1e-2f;
        r[i] = buffer[mid+i-pop] + rnd() * 1e-2f;
    }

    auto mse = math::emse(l, r);
    for(int i=0; i<=max; i++) mse[i] = mse[i+2*pop];
    mse.resize(max+1);
    for(float &i : mse) i = 1.0f - i;
    
    scale = pop;
    for(int i=min+1; i+1<=max; i++){
        if(mse[i] > mse[scale] && mse[i] > mse[i-1] && mse[i] > mse[i+1]){
            scale = i;
        }
    }

    similarity = mse[scale];
}

int Phaser2::match(int x, int y){

    for(int d=0, s=1; std::abs(d)<scale/2; d = -(d+s), s *= -1){
        if(std::abs((x+d)-mid) < max){
            const float e0 = buffer[y+d-1]-buffer[x-1];
            const float e1 = buffer[y+d]-buffer[x];
            const float e2 = buffer[y+d+1]-buffer[x+1];
            if(e0*e0 + e1*e1 + e2*e2 < 1e-2f) return y+d;
        }
    }

    return y;
}

}

