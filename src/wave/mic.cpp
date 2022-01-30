#include "wave/mic.h"

#include "wave/util.h"

#include <iostream>

namespace wave {

Mic::Mic(){
    setProcessingInterval(sf::milliseconds(1));
}

Mic::~Mic(){}

bool Mic::onStart(){
    
    reset();

    return 1;
}

bool Mic::onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount){

    push(int_to_float(samples, sampleCount));

    return 1;
}

void Mic::onStop(){}

}

