#include <algorithm>
#include <math.h>

#include "FIR.h"

FIR::FIR(){ this->length = 0; }

FIR::FIR(int32_t length_){ this->length = length_; }

bool FIR::resample(std::vector<float> *waves, double ratio, double filterCoefficient){

    int32_t wsize = waves->size();

    if(std::ceil(ratio*wsize) >= (double)(1ll<<31)) return 0;

    double conv = ratio*filterCoefficient;
    double iconv = 1/conv;

    int32_t nsize = (int32_t)std::ceil(ratio*wsize);
    std::vector<float> nwaves(nsize, 0);

    for(int32_t i=0; i<wsize; i++){
        
        double pos = ratio*i;
        int32_t left = std::max(0, (int32_t)std::ceil(pos-conv*this->length));
        int32_t right = std::min(nsize-1, (int32_t)std::floor(pos+conv*this->length));

        for(int32_t j=left; j<right; j++){
            nwaves[j] += (*waves)[i]*this->get(((double)j-pos)*iconv);
        }
    }

    waves->swap(nwaves);

    return 1;

}

bool FIR::resample(float *&waves, int32_t wsize, double ratio, double filterCoefficient){

    if(std::ceil(ratio*wsize) >= (double)(1ll<<31)) return 0;

    double conv = ratio*filterCoefficient;
    double iconv = 1/conv;

    int32_t nsize = (int32_t)std::ceil(ratio*wsize);
    float *nwaves = new float[nsize]();

    for(int32_t i=0; i<wsize; i++){
        
        double pos = ratio*i;
        int32_t left = std::max(0, (int32_t)std::ceil(pos-conv*this->length));
        int32_t right = std::min(nsize-1, (int32_t)std::floor(pos+conv*this->length));

        for(int32_t j=left; j<right; j++){
            nwaves[j] += waves[i]*this->get(((double)j-pos)*iconv);
        }
    }

    delete[] waves;
    waves = nwaves;

    return 1;

}

bool FIR::filter(std::vector<float> *waves, double coefficient){
    bool ret = this->resample(waves, 1, coefficient);
    if(ret) for(int32_t i=0; i<(int32_t)waves->size(); i++) (*waves)[i] /= coefficient;
    return ret;
}

bool FIR::filter(float *&waves, int32_t wsize, double coefficient){
    bool ret = this->resample(waves, wsize, 1, coefficient);
    if(ret) for(int32_t i=0; i<wsize; i++) waves[i] /= coefficient;
    return ret;
}

void FIR::impulse(std::vector<float> *waves, float amplitude, double pos, double conv){
 
    double iconv = 1.0/conv;

    int32_t left = std::max(0, (int32_t)std::ceil(pos-this->length*conv));
    int32_t right = std::min((int32_t)waves->size()-1, 
            (int32_t)std::floor(pos+this->length*conv));

    for(int32_t j=left; j<right; j++){
        (*waves)[j] += amplitude*this->get(((double)j-pos)*iconv);
    }
}

void FIR::impulse(float *waves, int32_t wsize, float amplitude, double pos, double conv){
 
    double iconv = 1.0/conv;

    int32_t left = std::max(0, (int32_t)std::ceil(pos-this->length*conv));
    int32_t right = std::min(wsize, (int32_t)std::floor(pos+this->length*conv));

    for(int32_t j=left; j<right; j++){
        waves[j] += amplitude*this->get(((double)j-pos)*iconv);
    }
}
