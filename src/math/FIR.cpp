#include "math/FIR.h"

#include <algorithm>
#include <math.h>

namespace math{

FIR::FIR(){ length = 0; }

FIR::FIR(int32_t length_){ length = length_; }

bool FIR::resample(std::vector<float> &waves, double ratio, double filterCoefficient){

    int32_t wsize = waves.size();

    if(std::ceil(ratio*wsize) >= (double)(1ll<<31)) return 0;

    double conv = ratio*filterCoefficient;

    std::vector<float> nwaves((int32_t)std::ceil(ratio*wsize), 0);

    for(int32_t i=0; i<wsize; i++){
        impulse(nwaves, waves[i], ratio*i, conv);
    }

    waves.swap(nwaves);

    return 1;

}

bool FIR::filter(std::vector<float> &waves, double coefficient){
    bool ret = resample(waves, 1, coefficient);
    if(ret) for(int32_t i=0; i<(int32_t)waves.size(); i++) waves[i] /= coefficient;
    return ret;
}

bool FIR::impulse(std::vector<float> &waves, float amplitude, double pos, double conv){
 
    double iconv = 1.0/conv;

    int32_t left = std::max(0, (int32_t)std::ceil(pos-length*conv));
    int32_t right = std::min((int32_t)waves.size()-1, 
            (int32_t)std::floor(pos+length*conv));

    for(int32_t j=left; j<=right; j++){
        waves[j] += amplitude*get((j-pos)*iconv);
    }

    return 1;
}

}

