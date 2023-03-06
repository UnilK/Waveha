#include "designf/pitcher.h"

#include "designf/common.h"
#include "designf/math.h"

#include <cassert>
#include <iostream>

namespace designf {

Pitcher::Pitcher(int frameRate, float minPitchHZ) :
    pacer(frameRate, minPitchHZ, 1200.0f),
    enveloper(64, 128),
    delay(enveloper.get_delay()),
    splicer(frameRate, minPitchHZ, 64),
    resampler(16)
{}

float Pitcher::process(float sample){

    pacer.push(sample);

    while(ready.empty()){
        
        sample = pacer.pull();

        float energy = enveloper.process(sample);
        sample = delay.process(sample);

        for(float i : splicer.process(sample, energy)){
            for(float j : resampler.process(i)) ready.push_back(j);
        }
    }

    sample = ready.front();
    ready.pop_front();

    return sample;
}

int Pitcher::get_delay(){
    return pacer.get_delay()
        + delay.get_delay()
        + splicer.get_delay()
        + resampler.get_delay();
}

void Pitcher::set_colored_shift(float shift){
    resampler.set_shift(shift);
}

void Pitcher::set_neutral_shift(float shift){
    splicer.set_shift(shift);
}

}

