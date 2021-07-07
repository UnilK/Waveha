#include "wave/voiceTransform.h"
#include "math/FFT.h"
#include "math/FT.h"
#include "math/constants.h"

#include <math.h>
#include <string.h>

VoiceTransform::VoiceTransform(
        float minFreq_, float maxFreq_,
        float minFit_, int32_t peaks_){
    this->set_tolerances(minFreq_, maxFreq_, minFit_, peaks_);
}

void VoiceTransform::set_tolerances(
        float minFreq_, float maxFreq_,
        float minFit_, int32_t peaks_){
    this->minFreq = minFreq_;
    this->maxFreq = maxFreq_;
    this->minFit = minFit_;
    this->peaks = peaks_;
}

float VoiceTransform::match_pitch(float frequency){

    /*
        assume that the spectrum is from a voice that has
        a fundamental frequency of {frequency}. Calculate
        that correlates to the sum of amplitudes of the
        fundamental & its subfrequencies.
    */

    float rf = frequency*this->block/this->dataFrequency;
    float sum = 0;

    for(int32_t i=1; rf*i<(float)this->block/2 && i<this->peaks; i++){
        int32_t pos = (int32_t)std::floor(rf*i);
        float d = rf*i-pos;
        sum += std::abs(spectrum[pos]*(1.0f-d)-spectrum[pos+1]*d);
    }

    return sum;

}

float VoiceTransform::find_pitch(){

    // identify the fundamental
    float maxAmp = 0, freq0 = 0, freq1, step = (this->maxFreq-this->minFreq)/this->block;
    for(float i=this->minFreq; i<=this->maxFreq; i+=step){
        float amp = this->match_pitch(i);
        if(amp > maxAmp){
            freq0 = i;
            maxAmp = amp;
        }
    }

    freq1 = freq0;
    maxAmp = 0;

    // enhance
    for(float i=freq0-0.5*step; i<freq0+0.5*step; i+=0.05*step){
        float amp = this->match_pitch(i);
        if(amp > maxAmp){
            freq1 = i;
            maxAmp = amp;
        }
    }

    return freq1;

}

void VoiceTransform::transform_audio(float *waves_, int32_t size_, int32_t dataFrequency_){

}

