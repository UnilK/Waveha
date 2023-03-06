#include "designf/splicer.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace designf {

Splicer::Splicer(int frameRate, float minPitchHZ, int minHopLength) :
    peakfinder(minHopLength),
    sdelay(peakfinder.get_delay()),
    edelay(peakfinder.get_delay())
{
    iwsize = std::ceil(frameRate / minPitchHZ);
    iwsize = std::max(minHopLength, iwsize);
    owsize = iwsize;
    hop = minHopLength;

    isize = 4 * iwsize;
    osize = 4 * owsize;

    in = 0;
    done = 0;
    state = 0;
    
    shift = 1.0f;
    out = 0.0f;

    ibuff.resize(isize, 0.0f);
    ebuff.resize(isize, 0.0f);
    pbuff.resize(isize, 0);
    obuff.resize(osize, 0.0f);
}

std::vector<float> Splicer::process(float sample, float energy){
    
    if(in + iwsize > isize){
        ibuff = ibuff.shift(in);
        ebuff = ebuff.shift(in);
        pbuff = pbuff.shift(in);
        out -= in;
        in = 0;
    }

    if(done + owsize > osize){
        obuff = obuff.shift(done);
        done = 0;
    }

    pbuff[in + iwsize - 1] = peakfinder.process(energy);
    ibuff[in + iwsize - 1] = sdelay.process(sample);
    ebuff[in + iwsize - 1] = edelay.process(energy);

    if(state <= 0){
        
        int next = hop;
        while(next + 1 < iwsize && !pbuff[in + next]) next++;

        int ileft = in, iright = in + next;
        int oleft = done, oright = done;
        
        {
            float i = out;
            while(i < iright){
                i += shift;
                oright++;
            }
        }
        
        int ilen = iright - ileft;
        int olen = oright - oleft;

        if(shift < 1.0f){

            // TODO

        }
        else {
            const float rot = M_PI / olen;
            for(int i=0; i<olen; i++){
                const float d = 0.5f + 0.5f * std::cos(rot * i);
                obuff[oleft + i] = ibuff[ileft + i] * d + ibuff[iright - olen + i] * (1.0f - d);
            }
        }

        state = next;
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
    
    shift = std::max(1.0f / 4, std::min(pitch_shift, 4.0f));
    owsize = std::max<int>(owsize, std::ceil(iwsize / shift));
    
    while(osize < 4 * owsize) osize *= 2;
    
    if(osize > (int)obuff.size()){
        auto prev = obuff;
        obuff.resize(osize, 0.0f);
        for(unsigned i=0; i<prev.size(); i++) obuff[i] = prev[i];
    }
}

int Splicer::get_delay(){
    return sdelay.get_delay() + iwsize - 1;
}

}

