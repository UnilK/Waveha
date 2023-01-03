#include "change/phaser2.h"
#include "math/constants.h"
#include "math/fft.h"

#include <cmath>
#include <complex>
#include <cassert>
#include <iostream>

namespace change {

Phaser2::Phaser2(int rate_, float low, float high){
    
    rate = rate_;

    if(low > high) std::swap(low, high);
    
    min = std::floor(rate/high);
    max = std::ceil(rate/low);
    
    wsize = (int)(rate / (3 * low)) / 8 * 8;
    if(wsize < 8) wsize = 8;

    fsize = (int)(rate / 1000) / 4 * 4;
    if(fsize < 4) fsize = 4;
    
    msize = std::max(wsize, fsize);
    
    size = 1;
    while(size < std::max(max, msize)) size *= 2;

    out = wstate = fstate = 0;
    in = 4 * size - 2 * msize;
    pass = 2 * size;

    ibuff.resize(4 * size, 0.0f);
    obuff.resize(8 * size, 0.0f);
    lowpass.resize(8 * size, 0.0f);
    window.resize(wsize);
    filter.resize(fsize);
    inv.resize(max+1);

    for(int i=0; i<wsize; i++) window[i] = (1.0 - std::cos(2 * PI * i / wsize)) / 8;
    for(int i=0; i<fsize; i++) filter[i] = (1.0 - std::cos(2 * PI * i / fsize)) / std::sqrt(6);
    for(int i=1; i<=max; i++) inv[i] = 1.0 / std::sqrt(i);
}

void Phaser2::push(float sample){ ibuff.push_back(sample); }

float Phaser2::pull(){

    if(in > 8 * size && in > (int)ibuff.size() / 2){
        int move = in - 2 * size;
        for(int i=0; i+move<(int)ibuff.size(); i++) ibuff[i] = ibuff[i+move];
        in -= move;
        ibuff.resize(ibuff.size()-move);
    }

    if(out + wsize > (int)obuff.size()){
        for(int i=0; i<(int)obuff.size(); i++){
            if(i+out<(int)obuff.size()) obuff[i] = obuff[i+out];
            else obuff[i] = 0.0f;
        }
        out = 0;
    }
    
    if(pass + fsize > (int)lowpass.size()){
        int move = pass - 2 * size;
        for(int i=0; i<(int)obuff.size(); i++){
            if(i+move<(int)lowpass.size()) lowpass[i] = lowpass[i+move];
            else lowpass[i] = 0.0f;
        }
        pass -= move;
    }

    if(in + msize > (int)ibuff.size()) in -= period();
    else if(in <= (int)ibuff.size() - max - 2 * msize) in += period();

    if(wstate == 0){
        for(int i=0; i<wsize; i++) obuff[out+i] += ibuff[in+i] * window[i];
    }

    if(fstate == 0){
        float sum = 0.0f;
        for(int i=0; i<fsize; i++) sum += ibuff[in+i] * filter[i];
        sum /= fsize;
        for(int i=0; i<fsize; i++) lowpass[pass+i] += sum * filter[i];
    }

    wstate = (wstate + 1) % (wsize / 8);
    fstate = (fstate + 1) % (fsize / 4);

    in++;
    pass++;
    return obuff[out++];
}

int Phaser2::period(){

    std::vector<float> a(max), b(max);
    for(int i=0; i<max; i++){
        a[i] = lowpass[pass+i-2*max];
        b[i] = lowpass[pass+i-max];
    }

    auto c = math::correlation(a, b);

    int best = min;
    for(int i=min; i<=max; i++) c[max-i] *= inv[i];
    for(int i=min+1, j=max-min-1; i<max; i++, j--){
        if(c[j] > c[max-best] && c[j-1] < c[j] && c[j+1] < c[j]) best = i;
    }

    return best;
}

float Phaser2::pitch(){ return (float)rate / period(); }

std::vector<float> Phaser2::debug(){

    std::vector<float> a(max), b(max);
    for(int i=0; i<max; i++){
        a[i] = lowpass[pass+i-2*max];
        b[i] = lowpass[pass+i-max];
    }

    auto c = math::correlation(a, b);

    int best = max;
    for(int i=min; i<=max; i++) c[max-i] *= inv[i];
    for(int i=min+1, j=max-min-1; i<max; i++, j--){
        if(c[j] > c[max-best] && c[j-1] < c[j] && c[j+1] < c[j]) best = i;
    }
    for(int i=0; i<(int)c.size(); i++){
        if(max-i < min || max-i > max) c[i] = 0.0f;
    }

    return c;
}

}

