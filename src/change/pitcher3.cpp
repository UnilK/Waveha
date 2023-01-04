#include "change/pitcher3.h"
#include "math/constants.h"
#include "math/fft.h"
#include "change/resample.h"

#include <cmath>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace change {

Pitcher3::Pitcher3(double shift_, int rate_, float low){

    shift = shift_;
    rate = rate_;

    max = std::ceil(rate / low);

    smax = ((int)std::ceil(max / shift) + 1) / 8 * 8;
    wsize = smax / 2;
    ssize = smax + wsize / 4;

    size = std::ceil(ssize * shift);

    in = size;
    state = out = 0;
    previous.resize(2 * ssize, 0.0f);
    ibuff.resize(8 * size, 0.0f);
    obuff.resize(8 * wsize, 0.0f);
    
    window.resize(wsize);
    for(int i=0; i<wsize; i++) window[i] = 1.0f - std::cos(2 * PIF * i / wsize);
    for(float &i : window) i = i*i/6;
}

float Pitcher3::process(float sample){
    
    if(in + size > 8 * size){
        int move = in-size;
        for(int i=0; i<8*size; i++){
            if(i+move < 8*size) ibuff[i] = ibuff[i+move];
            else ibuff[i] = 0.0f;
        }
        in -= move;
    }
    
    if(out + wsize > 8 * wsize){
        for(int i=0; i<8*wsize; i++){
            if(i+out < 8*wsize) obuff[i] = obuff[i+out];
            else obuff[i] = 0.0f;
        }
        out = 0;
    }

    ibuff[in+size-1] = sample;

    if(state == 0){
        
        std::vector<float> bit(2*size);
        for(int i=0; i<2*size; i++) bit[i] = ibuff[in+i-size];

        auto shifted = resample(bit, shift);
        {
            assert((int)shifted.size() > 2 * ssize);
            int d = (shifted.size() - 2 * ssize) / 2;
            if(d != 0) for(int i=0; i+d<(int)shifted.size(); i++) shifted[i] = shifted[i+d];
            shifted.resize(2 * ssize);
        }

        auto c = math::correlation(previous, shifted);
        int n = c.size(), m = 2 * ssize;

        {
            double sum = 0.0, e;
            for(float i : shifted) sum += i*i;
            for(float i : previous) sum += i*i;

            c[0] /= sum;
            
            e = sum;
            for(int i=1; i<=smax/2; i++){
                e -= previous[m-i]*previous[m-i];
                e -= shifted[i-1]*shifted[i-1];
                c[n-i] /= e;
            }
            
            e = sum;
            for(int i=1; i<smax/2; i++){
                e -= shifted[m-i]*shifted[m-i];
                e -= previous[i-1]*previous[i-1];
                c[i] /= e;
            }
        }
        
        int offset = 0;

        for(int i=-smax/2; i<smax/2; i++){
            int j = (n+i)%n;
            if(c[offset] < c[j]) offset = j;
        }

        if(offset >= smax/2) offset = offset - n;

        for(int i=0; i<wsize; i++){
            obuff[out+i] +=  shifted[ssize+offset-wsize/2] * window[i];
        }

        previous.swap(shifted);
        for(int i=0, d=offset-wsize/4; i<2*ssize; i++){
            if(i+d >= 0 && i+d < 2*ssize) previous[i] = previous[i+d];
            else previous[i] = 0.0f;
        }
    }

    state = (state + 1) % (wsize / 4);

    in++;
    return obuff[out++];
}

}

