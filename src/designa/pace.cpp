#include "designa/pace.h"
#include "designa/math.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

namespace designa {

Pace::Pace(int rate, float low, float high, float calc_frequency){
    
    if(low > high) std::swap(low, high);

    pop = std::max<int>(8, std::ceil(rate / (2 * 200.0f)));
    min = std::floor(rate/high);
    max = std::ceil(rate/low);

    min = std::min(pop, std::max(8, min));
    max = std::max(2 * pop, max);

    size = 4 * max + 2 * pop;
    buffer.resize(size, 0.0f);
   
    mid = max + pop;
    strong = mid;
    weak = -1;

    scale = pop;
    similarity = 0.0f;

    calc_state = 0;
    calc_period = std::ceil(rate / calc_frequency);

    decay = std::pow(epsilon, 1.0f / (rate * 0.01f));
    strong_weight = 1.0f;
    weak_weight = 0.0f;
}

void Pace::push(float sample){
    
    if(mid + max + pop > size){
        int move = mid - max - pop;
        buffer = buffer.shift(move);
        mid -= move;
        strong -= move;
        weak -= move;
    }

    buffer[mid+max+pop-1] = sample;

    if(calc_state == 0) calc_scale();
    calc_state = (calc_state + 1) % calc_period;
    
    strong_weight = strong_weight * decay + 1.0f - decay;
    weak_weight = weak_weight * decay;

    mid++;
}

float Pace::pull(){
    
    if(std::abs(mid-strong) >= max+pop){
        strong = mid;
        weak = -1;
        weak_weight = strong_weight = 0.0f;
    }
    
    if(3 * std::abs(strong - mid) > 2 * scale && strong_weight > 1.0f - epsilon){
        weak = strong;
        if(strong > mid) while(strong > mid) strong -= scale;
        else while(strong < mid) strong += scale;
        weak_weight = strong_weight;
        strong_weight = 0.0f;
    }
    
    strong_weight = strong_weight * decay + 1.0f - decay;
    weak_weight = weak_weight * decay;

    float sample = buffer[strong++] * strong_weight;
    if(std::abs(mid - weak) < max + pop) sample += buffer[weak++] * weak_weight;

    return sample;
}

int Pace::get_scale(){ return scale; }

float Pace::get_similarity(){ return similarity; }

int Pace::get_delay(){ return max + pop; }

void Pace::calc_scale(){

    std::vector<float> l(max+2*pop), r(max+2*pop);
    for(int i=0; i<max+2*pop; i++){
        l[i] = buffer[mid+i-max-pop] + rnd(0.01);
        r[i] = buffer[mid+i-pop] + rnd(0.01);
    }

    auto mse = energy_mse(l, r);
    for(int i=0; i<=max; i++) mse[i] = mse[i+2*pop];
    mse.resize(max+1);
    for(float &i : mse) i = 1.0f - i;
    
    scale = pop;
    for(int i=min+1; i+1<=max; i++){
        if(mse[i] > mse[scale] && mse[i] > mse[i-1] && mse[i] > mse[i+1]){
            scale = i;
        }
    }

    similarity = std::max(0.0f, std::min(mse[scale], 1.0f));
}

}

