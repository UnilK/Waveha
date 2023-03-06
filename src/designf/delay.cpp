#include "designf/delay.h"

#include <cassert>

namespace designf {

Delay::Delay(){
    resize(0);
}

Delay::Delay(int delayInSamples){
    resize(delayInSamples);
}

void Delay::resize(int delayInSamples){
    
    assert("delay must be non-negative" && delayInSamples >= 0);

    pointer = 0;
    delay = delayInSamples;
    size = 4 * delay;
    
    if(size == 0) size = 4;
    
    buffer.resize(size);
}

float Delay::process(float sample){

    if(pointer + delay >= size){
        buffer = buffer.shift(pointer);
        pointer = 0;
    }

    buffer[pointer + delay] = sample;

    return buffer[pointer++];
}

int Delay::get_delay(){ return delay; }

}

