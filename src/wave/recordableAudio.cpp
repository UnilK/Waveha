#include "wave/recordableAudio.h"

#include "wave/typeConverter.h"

namespace wave{

RecordableAudio::RecordableAudio(){}

bool RecordableAudio::onStart(){
    reset();
    return 1;
}

bool RecordableAudio::onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount){

    std::vector<float> data;

    for(std::size_t i=0; i<sampleCount; i++){
        data.push_back(int_to_float(samples[i]));
    }

    return 1;
}

void RecordableAudio::onStop(){}

}

