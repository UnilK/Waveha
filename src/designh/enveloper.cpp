#include "designh/enveloper.h"

#include "designh/common.h"
#include "designh/math.h"

#include <cassert>

namespace designh {

Enveloper::Enveloper(int windowSize){

    wsize = windowSize;
    state = 0;

    assert("window size must be a power of 2" && (wsize&-wsize) == wsize);
    assert("window size must be greater than 4" && wsize > 4);
    
    ibuff.resize(wsize, 0.0f);
    obuff.resize(wsize, 0.0f);

    window = cos_window(wsize);
}

float Enveloper::process(float sample){
    
    ibuff.push(sample);
    obuff.push(0.0f);

    if(state == 0){
        
        std::vector<float> bit(wsize);
        for(int i=0; i<wsize; i++) bit[i] = window[i] * ibuff[i];
        
        auto freq = fft(bit);

        float energy = 0.0f;
        for(int i=2; i<=wsize/2; i++) energy += std::norm(freq[i]) * i * i;

        energy += freq[0].real() * std::abs(freq[0]);
        energy += freq[1].real() * std::abs(freq[1]);

        energy /= 6 * wsize * wsize;

        for(int i=0; i<wsize; i++) obuff[i] += window[i] * energy;

        state = wsize / 4;
    }

    state--;

    return obuff[0];
}

int Enveloper::get_delay(){
    return wsize;
}

}
