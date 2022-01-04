#include "wave/readableAudio.h"

#include "wave/typeConverter.h"

#include <iostream>

namespace wave{

bool ReadableAudio::good(){
    return goodBit && !(shouldStop && (goodSamples == 0));
}

bool ReadableAudio::onGetData(Chunk &data){
    
    uint32_t readAmount;
    
    switch(readMode){
        case 0:
            readAmount = get_read_max();
            break;
        case 1:
            readAmount = get_read_max() / 2;
            break;
        case 2:
            readAmount = dataBlock;
            break;
        default:
            readAmount = get_read_max();
            break;
    }

    std::vector<float> floatData;
    floatData = get_move(readAmount);

    std::vector<sf::Int16> converted = float_to_int(floatData);

    data.samples = converted.data();
    data.sampleCount = readAmount;

    if(goodSamples < readAmount) goodSamples = 0;
    else goodSamples -= readAmount;

    return good();
}

void ReadableAudio::refill(){}

void ReadableAudio::onSeek(sf::Time timeOffset){}

void ReadableAudio::seek_sample(uint32_t sample){}

std::vector<float> ReadableAudio::get_data(uint32_t amount, uint32_t begin){
    return std::vector<float>(amount, 0.0f);
}

}
