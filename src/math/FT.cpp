#include "math/FT.h"
#include "math/constants.hpp"

#include <math.h>

namespace math{

using std::vector;
using std::complex;

const int32_t N = 1<<17;

// precalc of e^2*pi*i/N
complex<float> exp[N];

struct initialize{
    initialize(){
        for(int32_t i=0; i<N; i++){
            exp[i] = std::polar(1.0f, 2.0f*PIF*i/N);
        }
    }
} init;



complex<float> get_exp(double num){
    return exp[(int32_t)std::floor((num-std::floor(num))*N)];
}


complex<float> extract_frequency(float *waves, int32_t size, double frequency){
    
    complex<float> sinusoid = {0, 0};

    for(int32_t i=0; i<size; i++){
        sinusoid += waves[i]*get_exp(frequency*i);
    }

    return std::conj(sinusoid);
}

complex<float> extract_frequency(vector<float> &waves, double frequency){
    return extract_frequency(waves.data(), waves.size(), frequency);
}

}

