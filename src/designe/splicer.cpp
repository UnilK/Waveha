#include "designe/splicer.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace designe {

Splicer::Splicer(int frameRate, float minPitchHZ){
    
    iwsize = std::ceil(frameRate / minPitchHZ);
    iwsize = std::max(64, iwsize);
    owsize = iwsize;
    
    isize = 4 * iwsize;
    osize = 4 * owsize;

    in = 0;
    done = 0;
    state = 0;
    
    shift = 1.0f;
    out = 0.0f;

    ibuff.resize(isize, 0.0f);
    ebuff.resize(isize, 0.0f);
    pbuff.resize(isize, 0.0f);
    obuff.resize(osize, 0.0f);
}

std::vector<float> Splicer::process(float sample, float energy, float period){
    
    if(in + iwsize > isize){
        ibuff = ibuff.shift(in);
        ebuff = ebuff.shift(in);
        pbuff = pbuff.shift(in);
        out -= in;
        in = 0;
    }

    if(done + iwsize + owsize > osize){
        obuff = obuff.shift(done);
        done = 0;
    }

    ibuff[in + iwsize - 1] = sample;
    ebuff[in + iwsize - 1] = energy;
    pbuff[in + iwsize - 1] = period;

    if(state <= 0){
        
        int ilen = 0;
        {
            int min = std::max<int>(pbuff[in] * 0.45, 32);
            int max = std::min<int>(pbuff[in] * 1.55, iwsize);

            ilen = (min + max) / 2;
            for(int i=min; i<=max; i++) if(ebuff[in+i] > ebuff[in+ilen]) ilen = i;
        }

        int olen = 0;
        {
            double i = out;
            while(i < in + ilen){
                i += shift;
                olen++;
            }
        }

        int milen = std::min(ilen, olen);

        for(int i=0; i<ilen; i++){
            
            const float d = 0.5f + 0.5f * std::cos((float)M_PI * i / ilen);
            
            assert(done + i < osize);
            assert(in + i < isize);

            obuff[done + i] += ibuff[in + i] * d;
        }
        
        for(int i=1; i<=milen; i++){
            
            const float d = 0.5f + 0.5f * std::cos((float)M_PI * i / milen);
            
            assert(done + i < osize);
            assert(in + i < isize);
            assert(in + ilen - i >= 0);
            assert(in + ilen - i < isize);
            assert(in + olen - i >= 0);
            assert(in + olen - i < osize);

            obuff[done + olen - i] += ibuff[in + ilen - i] * d;
        }

        state = ilen;
    }

    state--;

    std::vector<float> result;

    in++;
    while(out < in){
        out += shift;
        result.push_back(obuff[done++]);
    }

    return result;
}

void Splicer::set_shift(float pitch_shift){
    
    shift = std::max(0.25f, std::min(pitch_shift, 4.0f));
    owsize = std::max<int>(owsize, std::ceil(iwsize / shift));
    while(osize < 4 * owsize) osize *= 2;
    
    if(osize > (int)obuff.size()){
        auto prev = obuff;
        obuff.resize(osize, 0.0f);
        for(unsigned i=0; i<prev.size(); i++) obuff[i] = prev[i];
    }
}

int Splicer::get_delay(){
    return iwsize;
}

}

