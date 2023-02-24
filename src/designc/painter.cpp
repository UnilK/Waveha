#include "designc/painter.h"

#include "designc/common.h"
#include "designc/math.h"

#include <cassert>
#include <cmath>

namespace designc {

Painter::Painter(int windowSize){
    
    wsize = windowSize;
    
    assert("window size must be a power of 2" && (wsize&-wsize) == wsize);
    assert("window size must be greater than 4" && wsize > 4);

    fsize = wsize / 2 + 1;
    size = 4 * wsize;
    pointer = 0;
    state = 0;
    
    ibuff.resize(size, 0.0f);
    obuff.resize(size, 0.0f);
    
    shift.resize(fsize, 1.0f);
   
    window = cos_window(wsize);
}

float Painter::process(float sample){
    
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

        auto energy = extract_energy(freq);
        auto phase = extract_phase(freq);

        energy = shift_bins(energy, shift);

        freq = create_frequency(energy, phase);

        bit = inverse_fft(freq);
        for(int i=0; i<wsize; i++) obuff[pointer + i] += bit[i] * window[i];

    }

    state = (state + 1) % (wsize / 4);

    return obuff[pointer++] / 6;
}

int Painter::get_shift_size(){ return fsize; }

void Painter::set_shift(const std::vector<float> &new_shift){
    assert(new_shift.size() == shift.size());
    shift = new_shift;
}

int Painter::get_delay(){ return wsize; }

}

