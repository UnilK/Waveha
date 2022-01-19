#include "wave/mic.h"

#include "wave/util.h"

namespace wave {

Mic::Mic(){}

Mic::~Mic(){}

bool Mic::onStart(){
    
    reset();

    return 1;
}

bool Mic::onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount){

    lock.lock();

    buffer.resize(std::max(write + sampleCount, buffer.size()));

    for(std::size_t i=0; i<sampleCount; i++) buffer[write++] = int_to_float(samples[i]);
    
    lock.unlock();

    return 1;
}

void Mic::onStop(){}

}

