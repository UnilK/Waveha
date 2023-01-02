#include "change/pitcher2.h"
#include "math/constants.h"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>

namespace change {

Pitcher2::Pitcher2(double shift_, int size_){

    size = std::max(4, size_ / 4 * 4);
    shift = std::min(100.0, std::max(0.01, shift_));
    
    period = size / 4;
    phase = in = done = 0;
    out = 0;
    
    step = shift;
    ssize = std::ceil(size / step);

    isize = 8 * size;
    osize = 8 * ssize;

    ibuff.resize(isize);
    obuff.resize(osize);
    window.resize(size);

    for(int i=0; i<size; i++) window[i] = (1.0 - std::cos(2.0 * PI * i / size)) / std::sqrt(6);
    
    freqs = size / 2 + 1;
    w.resize(freqs);
    for(int i=0; i<freqs; i++){
        w[i].resize(size);
        for(int j=0; j<size; j++) w[i][j] = std::polar(1.0, -2.0 * PI * j * i / size);
    }
}

std::vector<float> Pitcher2::process(float sample){
    
    if(in+size > isize){
        for(int i=0; i<isize; i++){
            if(i+in < isize) ibuff[i] = ibuff[i+in];
            else ibuff[i] = 0.0f;
        }
        
        out = std::max(out - in, 0.0);
        in = 0;

    }

    if(done + ssize > osize){
        for(int i=0; i<osize; i++){
            if(i+done < osize) obuff[i] = obuff[i+done];
            else obuff[i] = 0.0f;
        }
        done = 0;
    }

    assert(in >= 0 && in+size-1 < isize);
    ibuff[in+size-1] = sample;

    if(phase == 0){

        std::vector<std::complex<float> > f(freqs, 0.0f);
        for(int i=0; i<freqs; i++){
            for(int j=0; j<size; j++){
                f[i] += w[i][j] * window[j] * ibuff[in + j] ;
            }
        }

        {
            int j = 0, k = 0;
            double p = out - in;
            const double scalar = (2.0 / std::sqrt(6.0)) / size;
            while(p < size){
                
                while(j+1 < p) j++;
                
                const std::complex<float> rot = std::polar(1.0f, 2 * PIF * (float)(p-j) / size);
                std::complex<float> ang = rot;
                
                float sum = f[0].real() / 2;
                for(int i=1; i+1<freqs; i++){
                    sum += (f[i] * std::conj(w[i][j]) * ang).real();
                    ang *= rot;
                }
                if(freqs > 1) sum += f[freqs-1].real() / 2;
                
                sum *= scalar * (1.0 - std::cos(2 * PI * p / size));
                obuff[done+(k++)] += sum;
                p += step;
            }
        }
    }

    phase = (phase + 1) % period;

    std::vector<float> result;

    in++;
    while(out < in){
        out += step;
        assert(done < osize);
        result.push_back(obuff[done++]);
    }

    return result;
}

int Pitcher2::delay(){ return size; }

}
