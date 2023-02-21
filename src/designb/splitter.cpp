#include "designb/splitter.h"
#include "designb/common.h"
#include "designb/math.h"

#include <cassert>
#include <iostream>

namespace designb {

Splitter::Splitter(int size_){

    assert(size_ > 1);

    bsize = size_;
    wsize = 1<<bsize;
    size = 4 * wsize;
    pointer = 0;
    state = 0;

    ibuff.resize(size, 0.0f);
    obuff.resize(bsize, std::valarray(0.0f, size));

    window = cos_window(wsize);
    
    wavelet.resize(wsize / 2 + 1);
    ctype.resize(bsize, 0);

    for(int i=0; i<bsize; i++){
        int width = band_right(i) - band_left(i) + 1;
        if(width > bsize && width > 8){
            if(i == bsize-1) ctype[i] = 2;
            else ctype[i] = 1;
        }
    }

    for(int i=0; i<bsize; i++){
        if(ctype[i] == 0){
            int left = band_left(i);
            int right = band_right(i);
            for(int j=left; j<=right; j++){
                wavelet[j] = cos_wavelet(wsize, j);
                for(auto &k : wavelet[j]) k /= wsize;
                if(j != 0) for(auto &k : wavelet[j]) k *= 2;
            }
        }
    }
}

std::valarray<float> Splitter::process(float sample){
    
    if(pointer + wsize > size){
        ibuff = ibuff.shift(pointer);
        for(auto &i : obuff) i = i.shift(pointer);
        pointer = 0;
    }

    ibuff[pointer + wsize - 1] = sample;

    if(state == 0){
        
        std::vector<float> bit(wsize);
        for(int i=0; i<wsize; i++) bit[i] = ibuff[pointer + i] * window[i];

        auto freq = fft(bit);

        for(int i=0; i<bsize; i++){
            
            if(ctype[i] == 0){
                
                int l = band_left(i);
                int r = band_right(i);

                for(int j=l; j<=r; j++){
                    for(int k=0; k<wsize; k++){
                        obuff[i][pointer + k] += (freq[j] * wavelet[j][k]).real();
                    }
                }
            
            } else if(ctype[i] == 1){

                int l1 = band_left(i);
                int r1 = band_right(i);
                int l2 = band_left(i+1);
                int r2 = band_right(i+1);

                std::vector<std::complex<float> > band1(freq.size(), 0.0f);
                std::vector<std::complex<float> > band2(freq.size(), 0.0f);
                
                for(int j=l1; j<=r1; j++) band1[j] = freq[j];
                for(int j=l2; j<=r2; j++) band2[j] = freq[j];

                auto [bit1, bit2] = inverse_fft(band1, band2);

                for(int j=0; j<wsize; j++){
                    obuff[i][pointer+j] += bit1[j] * window[j];
                    obuff[i+1][pointer+j] += bit2[j] * window[j];
                }

                i++;

            } else {

                int l = band_left(i);
                int r = band_right(i);

                std::vector<std::complex<float> > band(freq.size(), 0.0f);
                
                for(int j=l; j<=r; j++) band[j] = freq[j];

                auto ibit = inverse_fft(band);

                for(int j=0; j<wsize; j++){
                    obuff[i][pointer+j] += ibit[j] * window[j];
                }
            }
        }
    }

    state = (state + 1) % (wsize / 4);

    std::valarray<float> processed(bsize);
    for(int i=0; i<bsize; i++) processed[i] = obuff[i][pointer] / 6;

    pointer++;

    return processed;
}

double Splitter::middle_period(int band){
    assert(band >= 0 && band < bsize);
    if(band == 0) return wsize;
    return wsize / (band_left(band) * std::sqrt(2));
}

int Splitter::get_size(){
    return bsize;
}

int Splitter::get_delay(){
    return wsize;
}

int Splitter::band_left(int band){
    if(band == 0) return 0;
    return 1<<(band-1);
}

int Splitter::band_right(int band){
    if(band == 0) return 0;
    return (1<<band)-1;
}

}

