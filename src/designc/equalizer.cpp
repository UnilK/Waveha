#include "designc/equalizer.h"

#include "designc/common.h"
#include "designc/math.h"

#include <cassert>
#include <iostream>

namespace designc {

Equalizer::Equalizer(int windowSize){
    
    wsize = windowSize;
    fsize = wsize / 2 + 1;
    size = 4 * wsize;

    assert("window size must be a power of 2" && (wsize&-wsize) == wsize);
    assert("window size must be greater than 4" && wsize > 4);

    gain_x = 1;
    clean_x = 1;
    noise_x = 1;
    shuffle_x = 1;
    blur_x = 1;
    
    pointer = 0;
    state = 0;

    ibuff.resize(size, 0.0f);
    obuff.resize(size, 0.0f);

    window = cos_window(wsize);

    gain.resize(fsize, 1.0f);
    clean.resize(fsize, 0.0f);
    shuffle.resize(fsize, 0.0f);
    noise.resize(fsize, 0.0f);
    blur.resize(fsize, 0.0f);
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
        if(blur_x) apply_blur(freq);
        if(shuffle_x) apply_shuffle(freq);
        if(noise_x) apply_noise(freq);

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

void Equalizer::apply_shuffle(std::vector<std::complex<float> > &freq){
    for(int i=0; i<fsize; i++){
        freq[i] += std::polar<float>(std::abs(freq[i]) * rnd(shuffle[i]), M_PI * rnd(1));
    }
}

void Equalizer::apply_noise(std::vector<std::complex<float> > &freq){
    for(int i=0; i<fsize; i++) freq[i] += std::polar<float>(noise[i], M_PI * rnd(1));
}

void Equalizer::apply_blur(std::vector<std::complex<float> > &freq){

    std::vector<float> in(fsize), out(fsize, 0.0f);
    for(int i=0; i<fsize; i++) in[i] = std::norm(freq[i]);

    for(int i=0; i<fsize; i++){
        if(blur[i] > 0.0f){
            
            int l = std::max<int>(0, std::ceil(i - blur[i]));
            int r = std::min<int>(fsize-1, std::floor(i + blur[i]));
            
            float sum = 0.0f;
            for(int j=l; j<=r; j++)
                sum += 0.5f + 0.5f * std::cos(M_PI * (j-i) / blur[i]);
            
            sum = 1.0f / (sum + 1e-18f);

            for(int j=l; j<=r; j++)
                out[j] += sum * in[i] * (0.5f + 0.5f * std::cos(M_PI * (j-i) / blur[i]));
            
        } else {
            out[i] += in[i];
        }
    }

    for(auto &i : freq) i /= std::abs(i) + 1e-18f;
    for(int i=0; i<fsize; i++) freq[i] *= std::sqrt(out[i]);
}

void Equalizer::enable_gain(bool x){ gain_x = x; }

void Equalizer::enable_clean(bool x){ clean_x = x; }

void Equalizer::enable_noise(bool x){ noise_x = x; }

void Equalizer::enable_shuffle(bool x){ shuffle_x = x; }

void Equalizer::enable_blur(bool x){ blur_x = x; }

void Equalizer::set_gain(const std::vector<float> &new_gain){
    assert(new_gain.size() == gain.size());
    gain = new_gain;
}

void Equalizer::set_clean(const std::vector<float> &new_clean){
    assert(new_clean.size() == clean.size());
    clean = new_clean;
}

void Equalizer::set_noise(const std::vector<float> &new_noise){
    assert(new_noise.size() == noise.size());
    noise = new_noise;
}

void Equalizer::set_shuffle(const std::vector<float> &new_shuffle){
    assert(new_shuffle.size() == shuffle.size());
    shuffle = new_shuffle;
}

void Equalizer::set_blur(const std::vector<float> &new_blur){
    assert(new_blur.size() == blur.size());
    blur = new_blur;
}

int Equalizer::get_frequency_size(){ return fsize; }

int Equalizer::get_delay(){ return wsize; }

}

