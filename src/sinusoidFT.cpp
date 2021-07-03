#include <vector>
#include <complex>
#include <math.h>

#include "sinusoidFT.h"
#include "mathConstants.h"

void SinusoidFT::set_params(float f_, float w_, float a_, int32_t n_){
    this->f = f_*n_/this->dataFrequency;
    this->w = w_;
    this->a = a_;
    this->n = n_;
}

const float clickTolerance = 1e-3;

std::complex<float> SinusoidFT::get(float x){
    if(std::abs(x+f) < clickTolerance) return std::polar(a*n, w);
    return std::polar(a, w)*(1.0f-std::polar(1.0f, 2.0f*PIF*(x+f)))
        /(1.0f-std::polar(1.0f, 2.0f*PIF*(x+f)/n));
}

std::complex<float> SinusoidFT::inv_get(float x){
    if(std::abs(x+f) < clickTolerance) return std::polar(1.0f/(a*n), 0.0f);
    return (1.0f-std::polar(1.0f, 2.0f*PIF*(x+f)/n))
        /(1.0f-std::polar(1.0f, 2.0f*PIF*(x+f)))*a;
}

SinusoidFT::SinusoidFT(){}

SinusoidFT::SinusoidFT(int32_t dataFrequency_){
    this->set_frequency(dataFrequency_);
}

void SinusoidFT::set_frequency(int32_t dataFrequency_){
    this->dataFrequency = dataFrequency_;
}

std::complex<float> SinusoidFT::get(float x,
        float frequency, float phase, float amplitude, int32_t size){
    this->set_params(frequency*size/this->dataFrequency, phase, amplitude, size);
    return this->get(x);
}

void SinusoidFT::add_sinusoid(std::vector<std::complex<float> > &spectrum,
        float frequency, float phase, float amplitude){
    
    this->set_params(frequency, phase, amplitude, spectrum.size());

    for(int32_t i=0; i<this->n; i++){
        std::complex<float> eval = this->get(-(float)i)/2.0f;
        spectrum[i] += std::conj(eval);
        spectrum[(this->n-i)%this->n] += eval;
    }

}

void SinusoidFT::add_sinusoid(std::complex<float> *spectrum, int32_t size,
        float frequency, float phase, float amplitude){

    this->set_params(frequency, phase, amplitude, size);

    for(int32_t i=0; i<this->n; i++){
        std::complex<float> eval = this->get(-(float)i)/2.0f;
        spectrum[i] += std::conj(eval);
        spectrum[(this->n-i)%this->n] += eval;
    }

}

std::complex<float> SinusoidFT::get_sinusoid(std::complex<float> left, std::complex<float> right){

    /*
        the peak lies somewhere between two frequencies.
        since there's quite a lot of spectral noise in human speech,
        taking an average might provide better results. The value
        closer to the peak has a larger amplitude, so it should
        have proportionally less noise.
    */


    float floorf = std::floor(this->f);
    float d = this->f-floorf;

    return 2.0f*(left*this->inv_get(-floorf)*(1.0f-d)+right*this->inv_get(-floorf-1.0f)*d);
}

std::complex<float> SinusoidFT::get_sinusoid(
        std::vector<std::complex<float> > &spectrum, float frequency){
    
    this->set_params(frequency, 0.0f, 1.0f, spectrum.size());
    int32_t pos = (this->n-(int32_t)std::floor(this->f))%this->n;
    return this->get_sinusoid(spectrum[pos], spectrum[(pos-1+this->n)%this->n]);
}

std::complex<float> SinusoidFT::get_sinusoid(
        std::complex<float> *spectrum, int32_t size, float frequency){

    this->set_params(frequency, 0.0f, 1.0f, size);
    int32_t pos = (this->n-(int32_t)std::floor(this->f))%this->n;
    return this->get_sinusoid(spectrum[pos], spectrum[(pos-1+this->n)%this->n]);
}

const float slope = 1.2f;

float SinusoidFT::evaluate_sinusoid(std::complex<float> left, std::complex<float> right){

    float floorf = std::floor(this->f);
    float d = (this->f-floorf)*slope-(slope-1.0f)*0.5f;

    return std::abs(left)*(1.0f-d)+std::abs(right)*d;
}

float SinusoidFT::evaluate_sinusoid(
        std::vector<std::complex<float> > &spectrum, float frequency){
    
    this->set_params(frequency, 0.0f, 1.0f, spectrum.size());
    int32_t pos = (this->n-(int32_t)std::floor(this->f))%this->n;
    return this->evaluate_sinusoid(spectrum[pos], spectrum[(pos-1+this->n)%this->n]);
}

float SinusoidFT::evaluate_sinusoid(
        std::complex<float> *spectrum, int32_t size, float frequency){

    this->set_params(frequency, 0.0f, 1.0f, size);
    int32_t pos = (this->n-(int32_t)std::floor(this->f))%this->n;
    return this->evaluate_sinusoid(spectrum[pos], spectrum[(pos-1+this->n)%this->n]);
}
