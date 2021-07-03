#include <math.h>
#include <string.h>

#include "voiceTransform.h"
#include "mathConstants.h"



VoiceTransform::VoiceTransform(){}

VoiceTransform::VoiceTransform(int32_t dataFrequency_, int32_t N_){
    this->set_frequency(dataFrequency_);
    this->N = N_;
}

void VoiceTransform::set_frequency(float dataFrequency_){
    this->dataFrequency = dataFrequency_;
    this->sft.set_frequency(dataFrequency_);
}

void VoiceTransform::set_size(int32_t N_){
    this->N = N_;
}

const int32_t PEAKS = 7;

float VoiceTransform::get_pitch_match(
        std::complex<float> *spectrum, float frequency, float offSet){

    /*
        assume that the spectrum is from a voice that has
        a fundamental frequency of {frequency}. Calculate
        the sum of amplitudes of the fundamental & the
        subfrequencies.
    */

    float rf = frequency*this->N/this->dataFrequency;
    float roff = offSet*this->N/this->dataFrequency;

    float sum = 0;

    for(int32_t i=1; roff+rf*i<(float)this->N/2 && i<PEAKS; i++){
        sum += this->sft.evaluate_sinusoid(spectrum, this->N, frequency*i);
    }

    return sum;

}

float VoiceTransform::get_pitch(float *waves){

    std::complex<float> *spectrum = this->fft.dft(waves, this->N);

    // identify a fundamental or one of it's subfrequencies.
    float maxAmp = 0, coeff = 0, freq0 = 0;
    for(float i=this->minFreq; i<=this->maxFreq; i+=0.25f){
        float amp = this->get_pitch_match(spectrum, i);
        if(amp > coeff){
            freq0 = i;
            maxAmp = amp;
            coeff = amp;
        }
    }

    float freq1 = freq0;

    /*
    // identify which subfrequency was found

    float bestTotal = 0;
    for(int i=2; i<10 && freq0/i >= this->minFreq; i++){
        float total = 0, sfreq = freq0/i;
        for(int j=1; j<i; j++) total += this->get_pitch_match(spectrum, freq0, sfreq*j);
        total /= (i-1);
        if(total/maxAmp >= this->subfreqLimit && total>bestTotal){
            freq1 = freq0/i;
            bestTotal = total;
        }
    }
    */

    float freq2 = freq1;
    maxAmp = 0;

    /*
    // enhance
    for(float i=freq1-5.0f; i<freq1+5.0f; i+=0.1f){
        float amp = this->get_pitch_match(spectrum, i);
        if(amp > maxAmp){
            freq2 = i;
            maxAmp = amp;
        }
    }
    */
    
    float freq3 = freq2;
    maxAmp = coeff = 0;

    // ENHANCE
    for(float i=freq2-0.2f; i<freq2+0.2f; i+=0.01f){
        float amp = this->get_pitch_match(spectrum, i);
        if(amp > maxAmp){
            freq3 = i;
            maxAmp = amp;
        }
    }

    delete[] spectrum;

    return freq3;

}

void VoiceTransform::set_tolerances(float min, float max){
    this->minFreq = min;
    this->maxFreq = max;
}

void VoiceTransform::transform_audio(float *waves, int32_t size){

}

