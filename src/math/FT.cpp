#include "math/FT.h"
#include "math/constants.hpp"

#include <math.h>

namespace math{
    
const int32_t RESOLUTION = 1<<17;
std::complex<float> exp[RESOLUTION] = {0};
std::complex<float> get_exp(double num);

struct initialize{
    initialize(){    
        for(int32_t i=0; i<RESOLUTION; i++){
            exp[i] = std::polar(1.0f, 2.0f*PIF*i/RESOLUTION);
        }
    }
} init;

std::complex<float> get_exp(double num){
    return exp[(int32_t)std::floor((num-std::floor(num))*RESOLUTION)];
}

std::complex<float> roll(std::vector<float> &waves, int32_t size, float frequency){

    std::complex<float> sinusoid = {0, 0};
    frequency /= size;

    for(int32_t i=0; i<size; i++){
        sinusoid += waves[i]*get_exp((double)i*frequency);
    }

    return std::conj(sinusoid);
}

}

