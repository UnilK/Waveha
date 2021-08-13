#include "wave/vocal.h"
#include "math/constants.h"

#include <math.h>
#include <algorithm>

namespace wave{

Vocal::Vocal(){
    size = 64;
    fund = 100.0f;
    set_size(size);
}

Vocal::Vocal(int32_t size_, float fundamental_){
    set_size(size_);
    set_fundamental(fundamental_);
}

void Vocal::set_size(int32_t size_){
    size = std::max(1, size_);
    voc.resize(size, {0, 0});
}

int32_t Vocal::get_size(){ return size; }

void Vocal::set_fundamental(float fundamental_){
    fund = fundamental_;
}

float Vocal::get_fundamental(){ return fund; }

void Vocal::set(int32_t freq, std::complex<float> state){
    if(freq < size) voc[freq] = state;
}

std::complex<float> Vocal::get(int32_t freq){
    if(freq < size) return voc[freq];
    return {0, 0};
}

void Vocal::clear(){
    shift({0.0f, 0.0f});
}

void Vocal::normalize(){
    double w = -std::arg(voc[0]);
    for(int32_t i=0; i<size; i++){
        voc[i] *= (std::complex<float>)std::polar(1.0, w*(i+1));
    }
}

void Vocal::shift(std::complex<float> state){
    for(int32_t i=0; i<size; i++){
        voc[i] *= state;
    }
}

void Vocal::set_phase(std::complex<float> state){
    shift(std::polar(1.0f, std::arg(state/voc[0])));
}

float Vocal::evaluate(float time){
    
    std::complex<float> phase = std::polar(1.0f, 2.0f*PIF*time*fund);
    std::complex<float> shift = phase, sum = {0, 0};

    for(auto i : voc){
        sum += i*shift;
        shift *= phase;
    }

    return 2.0f*sum.real();
}

}

