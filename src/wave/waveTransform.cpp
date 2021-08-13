#include "wave/waveTransform.h"
#include "math/FFT.h"
#include "math/FT.h"
#include "math/constants.h"
#include "math/sincFIR.h"

#include <math.h>
#include <string.h>

#include <iostream>

namespace wave{

WaveTransform::WaveTransform(
        int32_t dataFrequency_,
        float minFreq_, float maxFreq_, float minFit_,
        int32_t peaks_, int32_t freqs_){
    set_dataFrequency(dataFrequency_);
    set_tolerances(minFreq_, maxFreq_, minFit_, peaks_, freqs_);
}

WaveTransform::~WaveTransform(){}

bool WaveTransform::set_dataFrequency(int32_t dataFrequency_){
    dataFrequency = dataFrequency_;
    return 1;
}

bool WaveTransform::set_tolerances(
        float minFreq_, float maxFreq_, float minFit_,
        int32_t peaks_, int32_t freqs_){
    minFreq = minFreq_;
    maxFreq = maxFreq_;
    minFit = minFit_;
    peaks = peaks_;
    freqs = freqs_;
    return 1;
}

bool WaveTransform::roll_pitch(){
    
    return 1;
}

bool WaveTransform::init_core(){
    
    return 1;
}

bool WaveTransform::roll_core(){
 

    return 1;
}

bool WaveTransform::link_audio(std::vector<float> &audio){
    return link_audio(audio.data(), audio.size());
}

bool WaveTransform::link_audio(float *audio, int32_t size){
    
    block = 1;
    while(block < dataFrequency*minFit/minFreq) block *= 2;

    rpos = 0;
    previousPhase = {0, 0};

    source.resize(size);
    memcpy(source.data(), audio, sizeof(float)*size);

    clear_processed();

    return 1;
}

bool WaveTransform::add_audio(std::vector<float> &audio){
    return add_audio(audio.data(), audio.size());
}

bool WaveTransform::add_audio(float *audio, int32_t size){
    
    int32_t psize = source.size();
    source.resize(psize+size);
    memcpy(source.data()+psize, audio, sizeof(float)*size);
    
    return 1;
}


std::vector<float> WaveTransform::get_processed(){ return processed; }
std::vector<float> WaveTransform::get_source(){ return source; }
float *WaveTransform::processed_pointer(){ return processed.data(); }
float *WaveTransform::source_pointer(){ return source.data(); }
int32_t WaveTransform::processed_size(){ return processed.size(); }
int32_t WaveTransform::source_size(){ return source.size(); }
int32_t WaveTransform::write_position(){ return wpos; }
int32_t WaveTransform::read_position(){ return rpos; }


std::vector<float> WaveTransform::last_processed(int32_t size){

            // exploit the phase flip that occurs at the peak to differentiate
    if(size < 0) size = block;
    std::vector<float> waves(size, 0);
    for(int i=std::max(0, wpos-size); i<wpos; i++){
        waves[size+i-wpos] = processed[i];
    }
    return waves;
}

std::vector<float> WaveTransform::last_source(int32_t size){
    if(size < 0) size = block;
    std::vector<float> waves(size, 0);
    for(int i=std::max(0, rpos-size); i<rpos; i++){
        waves[size+i-rpos] = source[i];
    }
    return waves;
}

bool WaveTransform::clean_processed(){
    return 1;
}

bool WaveTransform::clean_source(){
    return 1;
}

bool WaveTransform::clear_processed(){
    processed.clear();
    wpos = 0;
    return 1;
}

bool WaveTransform::clear_source(){
    source.clear();
    return 1;
}

}

