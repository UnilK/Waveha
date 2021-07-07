#include "math/sincFIR.h"
#include "math/constants.h"

FastSincFIR::FastSincFIR(){
    this->length = 0;
    this->resolution = 0;
    this->size = 0;
}

FastSincFIR::FastSincFIR(int32_t length_, int32_t resolution_){
    this->initialize(length_, resolution_);
}

bool FastSincFIR::initialize(int32_t length_, int32_t resolution_){
    
    if((int64_t)length_*resolution_ >= 1ll<<31) return 0;
   
    this->length = length_;
    this->resolution = resolution_;
    this->size = length_*resolution_;

    this->sinc = new float[this->size];
    
	this->sinc[0] = 1;

    float coeff = PI/this->resolution;

	for(int32_t i=1; i<this->size; i++){
		this->sinc[i] = std::sin(coeff*i)/(coeff*i);
	}

	this->sinc[this->size-1] = 0;

    return 1;
}

float FastSincFIR::get(double num){
    // Caution!!! no segmentation fault gurads!!!
    return this->sinc[(int32_t)std::abs(num*this->resolution)];
}



PreciseSincFIR::PreciseSincFIR(){}

PreciseSincFIR::PreciseSincFIR(int32_t length_){ this->length = length_; }

float PreciseSincFIR::get(double num){
    return std::sin(PI*num)/(PI*num);
}

