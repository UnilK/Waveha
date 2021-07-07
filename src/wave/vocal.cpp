#include "wave/vocal.h"

#include <math.h>
#include <algorithm>

Vocal::Vocal(){
    this->size = 64;
    this->fund = 100.0f;
    this->set_size(this->size);
}

Vocal::Vocal(int32_t size_, float fundamental_){
    this->set_size(size_);
    this->set_fundamental(fundamental_);
}

void Vocal::set_size(int32_t size_){
    this->size = std::max(1, size_);
    this->voc.resize(this->size);
}

int32_t Vocal::get_size(){ return this->size; }

void Vocal::set_fundamental(float fundamental_){
    this->fund = fundamental_;
}

float Vocal::get_fundamental(){ return this->fund; }

void Vocal::set(int32_t freq, std::complex<float> state){
    if(freq < this->size) this->voc[freq] = state;
}

std::complex<float> Vocal::get(int32_t freq){
    if(freq < this->size) return this->voc[freq];
    return {0, 0};
}

void Vocal::clear(){
    this->shift({0.0f, 0.0f});
}

void Vocal::normalize(){
    double w = -std::arg(this->voc[0]);
    for(int32_t i=0; i<this->size; i++){
        this->voc[i] *= (std::complex<float>)std::polar(1.0, w*(i+1));
    }
}

void Vocal::shift(std::complex<float> state){
    for(int32_t i=0; i<this->size; i++){
        this->voc[i] *= state;
    }
}

void Vocal::set_phase(std::complex<float> state){
    this->shift(state/std::arg(this->voc[0]));
}

