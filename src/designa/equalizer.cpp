#include "designa/equalizer.h"
#include "designa/math.h"

#include <cassert>

namespace designa {

Equalizer::Equalizer(int window_size){
    
    wsize = window_size;
    fsize = wsize / 2 + 1;
    size = 4 * wsize;

    assert("window size must be a power of 2" && (wsize&-wsize) == wsize);

    merge_x = 1;
    gain_x = 1;
    clean_x = 1;
    noise_x = 1;
    shuffle_x = 1;
    
    pointer = 0;
    state = 0;

    pbuff.resize(size, 0.0f);
    rbuff.resize(size, 0.0f);
    obuff.resize(size, 0.0f);

    window = cos_window(wsize / 2, wsize / 2);

    merge.resize(fsize, 0.0f);
    gain.resize(fsize, 1.0f);
    clean.resize(fsize, 0.0f);
    shuffle.resize(fsize, 0.0f);
    noise.resize(fsize, 0.0f);
}

float Equalizer::process(float processed, float raw){
    
    if(pointer + wsize > size){
        pbuff = pbuff.shift(pointer);
        rbuff = rbuff.shift(pointer);
        obuff = obuff.shift(pointer);
        pointer = 0;
    }

    pbuff[pointer + wsize - 1] = processed;
    rbuff[pointer + wsize - 1] = raw;

    if(state == 0){
        
        std::vector<float> pbit(wsize), rbit(wsize);
        for(int i=0; i<wsize; i++){
            pbit[i] = pbuff[pointer + i] * window[i];
            rbit[i] = rbuff[pointer + i] * window[i];
        }

        auto [pfreq, rfreq] = fft(pbit, rbit);

        if(merge_x) apply_merge(pfreq, rfreq);
        if(gain_x) apply_gain(pfreq);
        if(clean_x) apply_clean(pfreq);
        if(shuffle_x) apply_shuffle(pfreq);
        if(noise_x) apply_noise(pfreq);

        pbit = inverse_fft(pfreq);
        for(int i=0; i<wsize; i++) obuff[pointer + i] += pbit[i] * window[i];
        
        state = wsize / 4;
    }

    state--;

    return obuff[pointer++] / 6;
}

void Equalizer::apply_merge(
        std::vector<std::complex<float> > &pfreq,
        std::vector<std::complex<float> > &rfreq)
{
    for(int i=0; i<fsize; i++){
        pfreq[i] = pfreq[i] * (1.0f - merge[i]) + rfreq[i] * merge[i];
    }
}

void Equalizer::apply_gain(std::vector<std::complex<float> > &freq){
    for(int i=0; i<fsize; i++) freq[i] *= gain[i];
}

void Equalizer::apply_clean(std::vector<std::complex<float> > &freq){
    for(int i=0; i<fsize; i++){
        float d = std::norm(freq[i]) / wsize;
        if(d < clean[i] && d > 0.0f){
            const float y = d / clean[i];
            freq[i] *= std::sqrt((y * y * clean[i]) / d);
        }
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

void Equalizer::enable_merge(bool x){ merge_x = x; }

void Equalizer::enable_gain(bool x){ gain_x = x; }

void Equalizer::enable_clean(bool x){ clean_x = x; }

void Equalizer::enable_noise(bool x){ noise_x = x; }

void Equalizer::enable_shuffle(bool x){ shuffle_x = x; }

void Equalizer::set_merge(const std::vector<float> &new_merge){
    assert(new_merge.size() == merge.size());
    merge = new_merge;
}

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

int Equalizer::get_frequency_size(){ return fsize; }

int Equalizer::get_delay(){ return wsize; }

}

