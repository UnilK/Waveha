#include "designe/splitter.h"

#include "designe/common.h"

#include <cassert>
#include <cmath>

namespace designe {

Splitter::Splitter(int windowSize){
    
    wsize = windowSize;
    
    assert("window size must be divisible by 8" && wsize % 8 == 0);
    assert("window size must be positive" && wsize > 0);

    fsize = wsize / 2 + 1;
    size = 4 * wsize;
    pointer = 0;
    state = 0;
    
    ibuff.resize(size, 0.0f);
    lbuff.resize(size, 0.0f);
    ebuff.resize(size, 0.0f);
    
    window = cos_window(wsize);
    wavelet = cos_wavelet(wsize, 1);
}

SplitSample Splitter::process(float sample){
    
    if(pointer + wsize > size){
        ibuff = ibuff.shift(pointer);
        lbuff = lbuff.shift(pointer);
        ebuff = ebuff.shift(pointer);
        pointer = 0;
    }

    ibuff[pointer + wsize - 1] = sample;

    if(state == 0){
        
        float f0 = 0.0f, esum = 0.0f;
        std::complex<float> f1 = 0.0f;
        
        for(int i=0; i<wsize; i++){
            const float s = ibuff[pointer + i] * window[i];
            f0 += s;
            f1 += ibuff[pointer + i] * std::conj(wavelet[i]);
            esum += s*s;
        }

        esum -= (std::norm(f0) + std::norm(f1) * 2) / wsize;
        
        f0 = f0 / wsize;
        f1 = 2.0f * f1 / (float)wsize;

        for(int i=0; i<wsize; i++){
            lbuff[pointer + i] += window[i] * f0 + (wavelet[i] * f1).real();
            ebuff[pointer + i] += esum * window[i];
        }
    }

    state = (state + 1) % (wsize / 8);

    return {
        ibuff[pointer],
        lbuff[pointer] / 12,
        ibuff[pointer] - lbuff[pointer] / 12,
        ebuff[pointer++] / 12,
    };
}

int Splitter::get_delay(){ return wsize; }

}

