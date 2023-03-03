#include "designe/pitcher.h"

#include "designe/common.h"
#include "designe/math.h"

#include <cassert>
#include <iostream>

namespace designe {

Pitcher::Pitcher(int frameRate, float minPitchHZ) :
    splitter(64),
    pacer(frameRate, minPitchHZ, 1200.0f),
    splicer(frameRate, minPitchHZ),
    resamplerLow(16),
    resamplerHigh(16)
{
    lowPipe.resize(splicer.get_delay(), 0.0f);
    
    colorShift = 1.0f;
    neutralShift = 1.0f;
    totalShift = 1.0f;
    
    resamplerLow.set_shift(totalShift);
    splicer.set_shift(neutralShift);
    resamplerHigh.set_shift(colorShift);
}

float Pitcher::process(float sample){
    
    pacer.push(sample);

    while(lowReady.empty() || highReady.empty()){

        sample = pacer.pull();

        auto split = splitter.process(sample);

        lowPipe.push_back(split.low);

        float low = lowPipe.front();
        lowPipe.pop_front();

        auto high = splicer.process(split.high, split.energy,
                pacer.get_real_period(), pacer.get_similarity());

        std::vector<float> lv = resamplerLow.process(low), hv;

        for(float i : high){
            auto j = resamplerHigh.process(i);
            for(float k : j) hv.push_back(k);
        }

        for(float i : lv) lowReady.push_back(i);
        for(float i : hv) highReady.push_back(i);
    }

    while(lowReady.size() + 5 < highReady.size()) highReady.pop_front();
    while(highReady.size() + 5 < lowReady.size()) lowReady.pop_front();

    sample = lowReady.front() + highReady.front();
    
    lowReady.pop_front();
    highReady.pop_front();

    return sample;
}

int Pitcher::get_delay(){
    return splitter.get_delay() + pacer.get_delay() + splicer.get_delay() +
        resamplerHigh.get_delay();
}

void Pitcher::set_colored_shift(float shift){
    colorShift = shift;
    update_shifts();
}

void Pitcher::set_neutral_shift(float shift){
    neutralShift = shift;
    update_shifts();
}

void Pitcher::update_shifts(){
    
    totalShift = neutralShift * colorShift;
    
    resamplerLow.set_shift(totalShift);
    splicer.set_shift(neutralShift);
    resamplerHigh.set_shift(colorShift);
}

}

