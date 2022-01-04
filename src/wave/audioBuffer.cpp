#include "wave/audioBuffer.h"

#include "wave/typeConverter.h"

#include <algorithm>
#include <iostream>

namespace wave {

AudioBuffer::AudioBuffer(uint32_t frameRate_, uint32_t channels_){
    frameRate = frameRate_;
    channels = channels_;
}



std::vector<float> AudioBuffer::get_move(uint32_t amount){
   
    audioLock.lock();
    
    std::vector<float> data(amount, 0.0f);
    
    for(uint32_t i=0; readPointer < audio.size() && i < amount; i++){
        data[i] = audio[readPointer++];
    }
    
    clean_audio();
    
    audioLock.unlock();

    return data;
}

std::vector<float> AudioBuffer::get_stationary(uint32_t amount){
    
    std::vector<float> data(amount, 0.0f);

    audioLock.lock();

    for(uint32_t i=0; readPointer + i < audio.size() && i < amount; i++){
        data[i] = audio[readPointer + i];
    }

    audioLock.unlock();

    return data;
}

void AudioBuffer::add_data(float *waves, uint32_t amount, int32_t writeOffset){
    
    audioLock.lock();

    audio.resize(std::max(writePointer + amount, (uint32_t)audio.size()), 0.0f);

    for(uint32_t i=0; i<amount; i++) audio[writePointer + i] += waves[i];

    if(writeOffset < 0) writeOffset = amount;

    writePointer += writeOffset;

    audioLock.unlock();
}

void AudioBuffer::add_data(std::vector<float> &waves, int32_t writeOffset){
    add_data(waves.data(), waves.size(), writeOffset);
}

bool AudioBuffer::satisfied(){
    audioLock.lock();
    bool sat = writePointer - readPointer >= preloadSize;
    audioLock.unlock();
    return sat;
}

uint32_t AudioBuffer::get_write_offset(){
    audioLock.lock();
    uint32_t offset = audio.size() - writePointer;
    audioLock.unlock();
    return offset;
}

uint32_t AudioBuffer::get_read_max(){
    audioLock.lock();
    uint32_t max = writePointer - readPointer;
    audioLock.unlock();
    return max;
}

uint32_t AudioBuffer::get_hunger(){
    if(satisfied()) return 0;
    audioLock.lock();
    uint32_t hunger = preloadSize - (writePointer - readPointer);
    audioLock.unlock();
    return hunger;
}

void AudioBuffer::clean_audio(){

    // should always be called inside audioLock

    if(readPointer * 2 > audio.size()){

        for(uint32_t i=0; i<audio.size() - readPointer; i++){
            audio[i] = audio[i + readPointer];
        }

        writePointer -= readPointer;
        audio.resize(audio.size() - readPointer);
        readPointer = 0;
    }
}

void AudioBuffer::reset(){
    audioLock.lock();
    
    audio.clear();
    readPointer = 0;
    writePointer = 0;

    audioLock.unlock();
}

}
