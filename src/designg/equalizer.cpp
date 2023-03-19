#include "designg/equalizer.h"

#include "designg/common.h"
#include "designg/math.h"

#include <cassert>
#include <iostream>

namespace designg {

Equalizer::Equalizer(int windowSize){
    
    wsize = windowSize;
    fsize = wsize / 2 + 1;
    size = 4 * wsize;

    assert("window size must be a power of 2" && (wsize&-wsize) == wsize);
    assert("window size must be greater than 4" && wsize > 4);

    gain_x = 0;
    clean_x = 0;
    
    pointer = 0;
    state = 0;

    ibuff.resize(size, 0.0f);
    obuff.resize(size, 0.0f);

    window = cos_window(wsize);

    gain.resize(fsize, 1.0f);
    clean.resize(fsize, 0.0f);
}

float Equalizer::process(float sample){
    
    if(pointer + wsize > size){
        ibuff = ibuff.shift(pointer);
        obuff = obuff.shift(pointer);
        pointer = 0;
    }

    ibuff[pointer + wsize - 1] = sample;

    if(state == 0){
        
        std::vector<float> bit(wsize);
        for(int i=0; i<wsize; i++) bit[i] = ibuff[pointer + i] * window[i];

        auto freq = fft(bit);
        for(auto &i : freq) i /= wsize;
        for(int i=1; i+1<fsize; i++) freq[i] *= 2; 

        if(clean_x) apply_clean(freq);
        if(gain_x) apply_gain(freq);

        for(auto &i : freq) i *= wsize;
        for(int i=1; i+1<fsize; i++) freq[i] /= 2; 
        
        bit = inverse_fft(freq);
        for(int i=0; i<wsize; i++) obuff[pointer + i] += bit[i] * window[i];
        
        state = wsize / 4;
    }

    state--;

    return obuff[pointer++] / 6;
}

void Equalizer::apply_gain(std::vector<std::complex<float> > &freq){
    for(int i=0; i<fsize; i++) freq[i] *= gain[i];
}

void Equalizer::apply_clean(std::vector<std::complex<float> > &freq){

    int width = fsize / 8;
    
    std::vector<float> abso(fsize);
    for(int i=0; i<fsize; i++) abso[i] = std::abs(freq[i]);

    double sum = 0.0f;
    for(int i=0, l=0, r=-1, len=0; i<fsize; i++){
        
        while(l < i - width){ sum -= abso[l++]; len--; }
        while(r < i + width && r+1 < fsize){ sum += abso[++r]; len++; }
        
        const float d = sum / len;

        if(d > 0.0f) freq[i] *= std::max(0.0f, d - clean[i]) / d;
    }
}

void Equalizer::enable_gain(bool x){ gain_x = x; }

void Equalizer::enable_clean(bool x){ clean_x = x; }

void Equalizer::set_gain(const std::vector<float> &new_gain){
    assert(new_gain.size() == gain.size());
    gain = new_gain;
}

void Equalizer::set_clean(const std::vector<float> &new_clean){
    assert(new_clean.size() == clean.size());
    clean = new_clean;
}

int Equalizer::get_frequency_size(){ return fsize; }

int Equalizer::get_delay(){ return wsize; }

}

