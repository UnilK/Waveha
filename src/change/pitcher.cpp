#include "change/pitcher.h"
#include "math/sinc.h"

#include <algorithm>
#include <cmath>

namespace change {

Pitcher::Pitcher(double shift_, int zeros_){

    shift = std::min(100.0, std::max(0.01, shift_));
    zeros = std::min(128, std::max(1, zeros_));

    step = 1.0 / shift;
    istep = shift;
    bsize = zeros * 8;
    in = zeros;
    out = 0;

    buffer.resize(bsize, 0.0f);
}

std::vector<float> Pitcher::process(float sample){
    
    if(in + zeros >= bsize){
        
        int move = out;
        
        in -= move;
        out -= move;

        for(int i=0; i<bsize; i++){
            if(i+move < bsize) buffer[i] = buffer[i+move];
            else buffer[i] = 0.0f;
        }
    }

    int l = std::floor(in), r = std::ceil(in);
    if(l == r) r++;

    double ll = (in-l)*istep, rr = (r-in)*istep;

    for(;l >= 0 && ll <= zeros; l--, ll+=istep) buffer[l] += sample * math::sinc(ll);
    for(;r < bsize && rr <= zeros; r++, rr+=istep) buffer[r] += sample * math::sinc(rr);

    std::vector<float> complete;
    while(out <= in - zeros) complete.push_back(buffer[out++]);

    in += step;

    return complete;
}

int Pitcher::delay(){ return zeros; }

}

