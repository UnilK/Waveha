#include "designd/painter.h"

#include "designd/common.h"
#include "designd/math.h"

#include <cassert>
#include <cmath>

namespace designd {

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

        std::vector<float> shifted(fsize, 0.0f);

        for(int i=0; i<fsize; i++){
            int j = std::round(i * shift[i]);
            if(j < fsize) shifted[j] += energy[i];
        }

        std::vector<float> blur1(fsize, 1e-18f), blur2(fsize, 1e-18f);
        
        int width = (fsize / 24) / 4 * 4;
        auto w = cos_window(width);

        for(int i=0; i<fsize; i+=width/4){
            
            float sum1 = 0.0f, sum2 = 0.0f;
            int len = 0;

            for(int j=-width/2; j<width/2; j++){
                if(i+j >= 0 && i+j < fsize){
                    len++;
                    sum1 += energy[i+j] * w[width/2 + j];
                    sum2 += shifted[i+j] * w[width/2 + j];
                }
            }

            sum1 /= len;
            sum2 /= len;
            
            for(int j=-width/2; j<width/2; j++){
                if(i+j >= 0 && i+j < fsize){
                    blur1[i+j] += sum1 * w[width/2 + j];
                    blur2[i+j] += sum2 * w[width/2 + j];
                }
            }
        }

        for(int i=0; i<fsize; i++) energy[i] *= blur2[i] / blur1[i];

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

int Painter::get_delay(){ return 0; }

}

