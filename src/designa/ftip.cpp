#include "designa/ftip.h"

#include <cassert>
#include <cmath>

namespace designa {

Ftip::Ftip(int size){

    assert((size % 4 == 0 && size > 0) && "resampling window size must be a multiple of 4");

    shift = 1;
    
    wsize = size / 2;
    osize = size;

    in = done = 0;
    out = 0;
    state = 0;
    
    ibuff.resize(8 * wsize, 0.0f);
    obuff.resize(8 * osize, 0.0f);

    wavelet.resize(wsize+1, std::vector<std::complex<float> >(2*wsize));
    for(int j=0; j<2*wsize; j++){
        for(int i=0; i<=wsize; i++){
            double abs = (1 - std::cos(M_PI * j / wsize)) / std::sqrt(6);
            double arg = - M_PI * j * i / wsize;
            wavelet[i][j] = std::polar(abs, arg);
        }
    }
}

std::vector<float> Ftip::process(float sample){
    
    if(in + 2 * wsize > (int)ibuff.size()){
        for(int i=0; i+in<(int)ibuff.size(); i++) ibuff[i] = ibuff[i + in];
        out -= in;
        in = 0;
    }

    if(done + osize > (int)obuff.size()){
        for(int i=0; i<(int)obuff.size(); i++){
            if(i + done < (int)obuff.size()) obuff[i] = obuff[i + done];
            else obuff[i] = 0.0f;
        }
        done = 0;
    }

    ibuff[in + 2 * wsize - 1] = sample;

    if(state == 0){

        std::vector<std::complex<float> > freq(wsize+1, 0.0f);
        for(int j=0; j<2*wsize; j++){
            for(int i=0; i<=wsize; i++){
                freq[i] += wavelet[i][j] * ibuff[in + j];
            }
        }

        int j = 0;
        float p = out - in;
        
        while(p < 2 * wsize){
            
            float sum = freq[0].real() * 0.5f;
            
            auto rot = std::polar<float>(1.0f, M_PI * p / wsize);
            auto ang = rot;
           
            for(int i=1; i<wsize; i++){
                sum += (freq[i] * ang).real();
                ang *= rot;
            }
            
            sum += (freq[wsize] * ang).real() * 0.5f;
            
            sum *= (1.0f - std::cos(M_PI * p / wsize)) / std::sqrt(6.0f) / wsize;
            
            obuff[done + j] += sum;
            p += shift;
            j++;
        }
    }

    state = (state + 1) % (wsize / 2);

    std::vector<float> result;

    in++;
    while(out < in){
        out += shift;
        result.push_back(obuff[done++]);
    }

    return result;
}

void Ftip::set_shift(float pitch_shift){
    shift = std::max(0.05f, std::min(pitch_shift, 20.0f));
    osize = std::max<int>(osize, 2 * wsize / shift);
    obuff.resize(8 * osize, 0.0f);
}

int Ftip::get_delay(){ return 2 * wsize; }

}

