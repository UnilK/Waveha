#include "wave/audioFile.h"
#include "wave/typeConverter.h"

#include <algorithm>
#include <string.h>
#include <iostream>

namespace wave{

AudioFile::AudioFile(std::string fileName){
    name = fileName;
    if(!file.open(name)) badBit = 1;
    else {
        frameRate = file.get_frame_rate();
        channels = file.get_channel_amount();
        initialize(1, frameRate);
    }
}

bool AudioFile::bad(){ return badBit; }

bool AudioFile::onGetData(Chunk &data){
    
    if(badBit) return 0; 

    readLock.lock();
    std::vector<float> floatData;
    uint32_t readAmount = file.read_move(floatData, frameRate*channels);
    readLock.unlock();

    parse_channel(floatData);

    converted = float_to_int(floatData);

    data.samples = converted.data();
    data.sampleCount = frameRate;

    return readAmount == frameRate*channels;
}

void AudioFile::onSeek(sf::Time timeOffset){
    readLock.lock();
    file.seek((uint32_t)(channels * (double)timeOffset.asSeconds() * frameRate));
    readLock.unlock();
}

void AudioFile::setChannel(int32_t channel_){
    channel = std::min(channels-1, channel_);
}

std::vector<float> AudioFile::peekData(uint32_t amount){
    return lookData(file.tell(), amount);
}

std::vector<float> AudioFile::lookData(uint32_t begin, uint32_t amount){
    readLock.lock();
    std::vector<float> data = file.read_silent(begin, amount*channels);
    readLock.unlock();
    
    parse_channel(data);
    return data;
}

void AudioFile::parse_channel(std::vector<float> &data){
    if(channels < 2) return;
    uint32_t total = 0;
    for(uint32_t i=channel; i<data.size(); i += channels){
        data[i/channels] = data[i];
        total++;
    }
    data.resize(total);
}

}

