#include "math/sincFIR.h"
#include "math/constants.h"

#include <math.h>

namespace math{

FastSincFIR::FastSincFIR(){
    length = 0;
    resolution = 0;
    size = 0;
}

FastSincFIR::FastSincFIR(int32_t length_, int32_t resolution_){
    initialize(length_, resolution_);
}

bool FastSincFIR::initialize(int32_t length_, int32_t resolution_){
    
    if((int64_t)length_*resolution_ >= 1ll<<31) return 0;
   
    length = length_;
    resolution = resolution_;
    size = length_*resolution_;

    sinc = new float[size];
    
	sinc[0] = 1;

    float coeff = PI/resolution;

	for(int32_t i=1; i<size; i++){
		sinc[i] = std::sin(coeff*i)/(coeff*i);
	}

	sinc[size-1] = 0;

    return 1;
}

float FastSincFIR::get(double num){
    return sinc[(int32_t)std::abs(num*resolution)];
}



PreciseSincFIR::PreciseSincFIR(){}

PreciseSincFIR::PreciseSincFIR(int32_t length_){ length = length_; }

float PreciseSincFIR::get(double num){
    return std::sin(PI*num)/(PI*num);
}

}

