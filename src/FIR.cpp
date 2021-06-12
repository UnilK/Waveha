#include <math.h>
#include <algorithm>

#include "FIR.h"


const double PI = 3.14159265358979323;

FIR::FIR(){}



SincFIR::SincFIR(){
    this->zeros = 0;
    this->resolution = 0;
    this->size = 0;
}

SincFIR::SincFIR(int32_t zeros_, int32_t resolution_){
    this->initialize(zeros_, resolution_);
}

bool SincFIR::initialize(int32_t zeros_, int32_t resolution_){
    
    if((int64_t)zeros_*resolution_ >= 1ll<<31) return 0;
   
    this->zeros = zeros_;
    this->resolution = resolution_;
    this->size = zeros_*resolution_;

    this->sinc = new float[this->size];
    
	this->sinc[0] = 1;

    float coeff = PI/this->resolution;

	for(int32_t i=1; i<this->size; i++){
		this->sinc[i] = std::sin(coeff*i)/(coeff*i);
	}

	this->sinc[this->size-1] = 0;

    return 1;
}

float SincFIR::get(double num){
    return this->sinc[std::min(this->size-1, (int32_t)std::abs(num*this->resolution))];
}

bool SincFIR::resample(std::vector<float> *waves,
                        double ratio, double filterCoefficient){

    int32_t wsize = waves->size();

    if(std::ceil(ratio*wsize) >= (double)(1ll<<31)) return 0;

    double conv = ratio*filterCoefficient;
    double iconv = 1/conv;

    int32_t nsize = (int32_t)std::ceil(ratio*wsize);
    std::vector<float> nwaves(nsize, 0);

    for(int32_t i=0; i<wsize; i++){
        
        double pos = ratio*i;
        int32_t left = std::max(0, (int32_t)std::ceil(pos-conv*this->zeros));
        int32_t right = std::min(nsize-1, (int32_t)std::floor(pos+conv*this->zeros));

        for(int32_t j=left; j<right; j++){
            nwaves[j] += (*waves)[i]*this->get(((double)j-pos)*iconv);
        }
    }

    waves->swap(nwaves);

    return 1;

}

bool SincFIR::resample(float *&waves, int32_t wsize,
                        double ratio, double filterCoefficient){

    if(std::ceil(ratio*wsize) >= (double)(1ll<<31)) return 0;

    double conv = ratio*filterCoefficient;
    double iconv = 1/conv;

    int32_t nsize = (int32_t)std::ceil(ratio*wsize);
    float *nwaves = new float[nsize]();

    for(int32_t i=0; i<wsize; i++){
        
        double pos = ratio*i;
        int32_t left = std::max(0, (int32_t)std::ceil(pos-conv*this->zeros));
        int32_t right = std::min(nsize-1, (int32_t)std::floor(pos+conv*this->zeros));

        for(int32_t j=left; j<right; j++){
            nwaves[j] += waves[i]*this->get(((double)j-pos)*iconv);
        }
    }

    delete[] waves;
    waves = nwaves;

    return 1;

}

bool SincFIR::filter(std::vector<float> *waves, double coefficient){
    return this->resample(waves, 1, coefficient);
}

bool SincFIR::filter(float *&waves, int32_t wsize, double coefficient){
    return this->resample(waves, 1, coefficient);
}

