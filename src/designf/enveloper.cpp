#include "designf/enveloper.h"

#include "designf/common.h"

#include <cassert>
#include <cmath>

namespace designf {

Enveloper::Enveloper(int minWindowSize, int maxWindowSize){
    
    assert("window sizes must be divisible by 8" && minWindowSize % 8 == 0);
    assert("window sizes must be positive" && minWindowSize > 0);
    assert(minWindowSize <= maxWindowSize);
  
    dsize = 0;
    for(int width=minWindowSize; width<=maxWindowSize; width*=2){
        wsize.push_back(width);
        state.push_back(0);
        window.push_back(cos_window(width));
        wavelet.push_back(cos_wavelet(width, 1));
        msize = width;
        dsize++;
    }

    size = 4 * msize;
    pointer = 0;
    
    ibuff.resize(size, 0.0f);
    obuff.resize(size, 0.0f);
}

float Enveloper::process(float sample){
    
    if(pointer + msize > size){
        ibuff = ibuff.shift(pointer);
        obuff = obuff.shift(pointer);
        pointer = 0;
    }

    ibuff[pointer + msize - 1] = sample;
    
    for(int i=0; i<dsize; i++){
        if(state[i] == 0){
            
            float a0 = 0.0f, e = 0.0f;
            std::complex<float> a1 = 0.0f;

            for(int j=0; j<wsize[i]; j++){
                const float &s = ibuff[pointer + j];
                a0 += s * window[i][j];
                e += s * s * window[i][j] * window[i][j];
                a1 += s * wavelet[i][j];
            }

            float sum = e - (a0 * a0 + 2 * std::norm(a1)) / wsize[i];
            sum /= wsize[i];

            for(int j=0; j<wsize[i]; j++){
                obuff[pointer + j] += sum * window[i][j];
            }
            
            state[i] = wsize[i] / 8;
        }
        state[i]--;
    }

    return obuff[pointer++] / (12 * dsize);
}

int Enveloper::get_delay(){ return msize - 1; }

}

