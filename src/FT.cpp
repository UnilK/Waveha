#include "FT.h"
#include "mathConstants.h"
#include <math.h>

FT::FT(){
    this->dataFrequency = 44100;
}

FT::FT(float dataFrequency_){
    this->dataFrequency = dataFrequency_;
}
        
std::complex<float> FT::roll(float *waves, int32_t size, float frequency){
    
    float f = 2.0f*PIF*frequency/this->dataFrequency, tot = 0;
    std::complex<float> soid = {0, 0};


    for(int32_t i=0; i<size; i++){
        tot += waves[i];
    }

    tot /= size;

    for(int32_t i=0; i<size; i++){
        soid += (waves[i]-tot)*std::polar(1.0f, f*i);
    }

    return soid;
}

const int N = 1;
float FT::calc_match(float *waves, int32_t size, float fundamental){
    
    float sum = 0;

    for(int32_t i=1; i<=N; i++){
        sum += std::abs(this->roll(waves, size, fundamental*i));
    }

    return sum/size;

}

