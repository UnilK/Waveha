#include "wave/audioClassifier.h"

#include <math.h>

namespace wave{

bool is_noise(
        std::vector<float> &waves,
        float tolerance){
    return is_noise(waves.data(), waves.size(), tolerance);
}


bool is_noise(
        float *waves,
        int32_t size,
        float tolerance){

    float avg0 = 0, avg1 = 0;
    for(int32_t i=0; i<size; i++) avg0 += waves[i];
    avg0 /= size;

    float sum = 0;
    for(int32_t i=1; i<size-1; i++){
        sum += std::abs(waves[i-1]+waves[i+1]-2*waves[i]);
        avg1 += std::abs(waves[i]-avg0);
    }
    avg1 = (waves[0]+waves[size-1]+avg1)/size;
    sum /= size*avg1;

    return sum > tolerance;
}

bool is_silent(
        std::vector<float> &waves,
        float tolerance){
    return is_silent(waves.data(), waves.size(), tolerance);
}

bool is_silent(
        float *waves,
        int32_t size,
        float tolerance){
    
    float avg0 = 0, avg1 = 0;
    for(int32_t i=0; i<size; i++) avg0 += waves[i];
    avg0 /= size;
    for(int32_t i=0; i<size; i++) avg1 += std::abs(waves[i]-avg0);
    avg1 /= size;
    return avg1 < tolerance;
}

}

