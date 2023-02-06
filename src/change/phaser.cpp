#include "change/phaser.h"
#include "math/fft.h"
#include "math/constants.h"

#include <cmath>
#include <cassert>
#include <iostream>

namespace change {

Phaser::Phaser(int rate, float low, float high){

    if(low > high) std::swap(low, high);

    low = std::max(low, 20.0f);
    high = std::min(high, 2000.0f);

    flen = std::ceil((double)rate/100.0f);
    padding = std::max<int>(2 * wlen, std::ceil((double)rate/400.0f));

    min = std::floor(rate/high);
    min = std::max(min, 2 * wlen);
    max = std::ceil(rate/low);
    max = std::max(2*padding, max);
    if(min > max) max = min;

    mid = max;
    in = mid;
    out = mid;
    done = 0;

    shift = 1.0f;
    owlen = 2 * wlen;

    scale = min;
    similarity = 0.0f;
    
    wstate = fstate = 0;
    
    ibuff.resize(2 * wlen + 8 * max, 0.0f);
    obuff.resize(8 * owlen, 0.0f);
    
    mse.resize(max+1, 0.0f);

    window.resize(2 * wlen);
    for(int i=0; i<2*wlen; i++) window[i] = (1.0f - std::cos(PIF * i / wlen)) / std::sqrt(6.0);

    wavelet.resize(wlen, std::vector<std::complex<float> >(2 * wlen));
    for(int i=0; i<wlen; i++){
        for(int j=0; j<2*wlen; j++){
            wavelet[i][j] = std::polar(window[j], PIF * i * j / wlen);
        }
    }
}

float Phaser::process(float sample){
    
    if(mid+max+padding > (int)ibuff.size()){
        int move = mid - max - padding;
        for(int i=0; i+move<(int)ibuff.size(); i++) ibuff[i] = ibuff[i+move];
        mid -= move;
        out -= move;
        in -= move;
    }

    if(done + owlen >= (int)obuff.size()){
        for(int i=0; i<(int)obuff.size(); i++){
            if(i+done < (int)obuff.size()) obuff[i] = obuff[i+done];
            else obuff[i] = 0.0f;
        }
        done = 0;
    }

    ibuff[mid+max+2*wlen-1] = sample;

    if(fstate == 0) find_scale();
    fstate = (fstate + 1) % flen;

    // paranoid saftey measures
    if(in-mid > max){ in -= max; out -= max; }
    if(mid-in > max){ in += max; out += max; }

    out += shift;
    while(out > in){
        
        if(wstate == 0){
            
            std::vector<float> bit(2 * wlen);

            // normilize pace - balance around middle.

            if(std::abs(in - mid) > 2*scale/3 &&
                    ((shift > 1.0f && in > mid) ||
                    (shift < 1.0f && in < mid))){
               
                int old = in, dir = (in < mid) - (in > mid);

                if(similarity > 0.3f){

                    in = std::round(in + scale * dir);

                    int best = 0;
                    float top = 0.0f;
                    for(int i=0; i<2*wlen; i++){
                        const float error = (ibuff[in+i]-ibuff[old+i]);
                        top += error*error*window[i];
                    }

                    for(int d=-2; d<=2; d++){
                        float sum = 0.0f;
                        for(int i=0; i<2*wlen; i++){
                            const float error = (ibuff[in+d+i]-ibuff[old+i]);
                            sum += error*error*window[i];
                        }
                        if(sum < top){
                            top = sum;
                            best = d;
                        }
                    }

                    in += best;

                } else {
                     
                    int best = min;
                    float top = 0.0f;
                    for(int i=0; i<2*wlen; i++){
                        const float error = (ibuff[in+dir*min+i]-ibuff[old+i]);
                        top += error*error*window[i];
                    }

                    for(int d=min+1; d<=2*padding; d++){
                        float sum = 0.0f;
                        for(int i=0; i<2*wlen; i++){
                            const float error = (ibuff[in+d*dir+i]-ibuff[old+i]);
                            sum += error*error*window[i];
                        }
                        if(sum < top){
                            top = sum;
                            best = d;
                        }
                    }

                    in += best*dir;
                }
                
                out += in - old;

                for(int i=0; i<2*wlen; i++){
                    float d = (float)i / (2 * wlen);
                    bit[i] = ibuff[old+i] * (1.0f - d) + ibuff[in+i] * d;
                }

            } else {
                for(int i=0; i<2*wlen; i++) bit[i] = ibuff[in+i];
            }

            // resample with wavelets

            std::vector<std::complex<float> > freq(wlen, 0.0f);
            for(int j=0; j<2*wlen; j++){
                for(int i=0; i<wlen; i++){
                    freq[i] += bit[j] * wavelet[i][j];
                }
            }

            for(auto &i : freq) i /= 2 * wlen;

            int j = 0;
            float p = out - in;
            const double scalar = (2.0 / std::sqrt(6.0));
            
            while(p < 2 * wlen){
                
                const std::complex<float> rot = std::polar(1.0f, PIF * p / wlen);
                std::complex<float> ang = rot;
                
                float sum = freq[0].real() / 2;
                for(int i=1; i+1<wlen; i++){
                    sum += (freq[i] * ang).real();
                    ang *= rot;
                }
                sum += (freq[wlen-1] * rot).real() / 2;
                
                sum *= scalar * (1.0f - std::cos(PIF * p / wlen));
                obuff[done+(j++)] += sum;
                p += shift;
            }
        }

        wstate = (wstate + 1) % (wlen / 2);
        in++;
    }

    mid++;
    return obuff[done++];
}

void Phaser::set_shift(float pitch_shift){
    shift = std::max(0.05f, std::min(pitch_shift, 20.0f));
    owlen = std::ceil(2 / shift * wlen);
    obuff.resize(std::max((int)obuff.size(), 8 * owlen), 0.0f);
}

float Phaser::get_scale(){ return scale; }

float Phaser::get_similarity(){ return similarity; }

int Phaser::delay(){ return max + padding; }

std::vector<float> Phaser::debug(){ return mse; }

void Phaser::find_scale(){

    std::vector<float> left(max+2*padding), right(max+2*padding);
    for(int i=0; i<max+2*padding; i++){
        left[i] = ibuff[mid+i-max-padding];
        right[i] = ibuff[mid+i-padding];
    }

    mse = math::emse(left, right);
    for(int i=0; i<=max; i++) mse[i] = mse[i+2*padding];
    mse.resize(max+1);
    for(float &i : mse) i = 1.0f - i;
    
    int best = min; float top = 0.0f;
    for(int i=min+1; i+1<=max; i++){
        if(mse[i] > top && mse[i] > mse[i-1] && mse[i] > mse[i+1]){
            best = i;
            top = mse[i];
        }
    }
    
    auto para = [](float i, float l, float m, float r) -> float{
        const float a = 0.5f * (r+l) - m;
        const float b = (r-l) * 0.5f;
        const float d = - b / (2 * a);
        return i+d;
    };

    if(best > min){
        scale = para(best, mse[best-1], mse[best], mse[best+1]);
        similarity = std::min(1.0f, std::max(0.0f, mse[best]));
    }
    else {
        scale = padding;
        similarity = 0.0f;
    }
}

void Phaser::check_scale(){
    
}

}

