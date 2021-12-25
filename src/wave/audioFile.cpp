#include "wave/audioFile.h"
#include "wave/typeConverter.h"

#include <algorithm>
#include <string.h>
#include <iostream>

namespace wave{

AudioFile::AudioFile(){}

AudioFile::AudioFile(std::string fileName){
    open(fileName);
}

bool AudioFile::open(std::string fileName){
    goodBit = 1;
    name = fileName;
    if(!file.open(name)) goodBit = 0;
    else {
        frameRate = file.get_frame_rate();
        channels = file.get_channel_amount();
        initialize(channels, frameRate);
    }
    return goodBit;
}

void AudioFile::onSeek(sf::Time timeOffset){
    seek_sample((uint32_t)(channels * (double)timeOffset.asSeconds() * frameRate));
}

void AudioFile::seek_sample(uint32_t sample){
    file.seek(sample);
    reset();
    refill();
}

void AudioFile::refill(){

    if(!goodBit) return;

    uint32_t target = get_hunger();
    std::vector<float> samples;
    uint32_t actual = file.read_move(samples, target);
   
    if(target != actual){
        goodSamples = actual;
        shouldStop = 1;
    }

    add_data(samples);
}



FixedAudioBuffer::FixedAudioBuffer(){}

FixedAudioBuffer::FixedAudioBuffer(const std::vector<float> &audio_,
        uint32_t channels_, uint32_t frameRate_){

    audio = audio_;
    channels = channels_;
    frameRate = frameRate_;

}

FixedAudioBuffer::FixedAudioBuffer(std::string fileName){
    
    iwstream file;

    if(file.open(fileName)){
        frameRate = file.get_frame_rate();
        channels = file.get_channel_amount();
        audio = file.read_file();
    }
}



LoadedAudioFile::LoadedAudioFile(){}

LoadedAudioFile::LoadedAudioFile(FixedAudioBuffer *buff_){
    open(buff_);
}

bool LoadedAudioFile::open(FixedAudioBuffer *buff_){
    goodBit = 1;
    buff = buff_;
    initialize(buff->channels, buff->frameRate);
    return goodBit;
}

void LoadedAudioFile::onSeek(sf::Time timeOffset){
    seek_sample((uint32_t)(channels * (double)timeOffset.asSeconds() * frameRate));
}

void LoadedAudioFile::seek_sample(uint32_t sample){
    position = sample;
    reset();
    refill();
}

void LoadedAudioFile::refill(){

    if(!goodBit) return;

    uint32_t target = get_hunger();
    std::vector<float> samples(target, 0.0f);
    
    uint32_t actual = 0;
    if(buff->audio.size() >= position)
        actual = std::min((uint32_t)buff->audio.size() - position, target);

    for(uint32_t i = 0; i < actual; i++) samples[i] = buff->audio[position++];

    if(target != actual){
        goodSamples = actual;
        shouldStop = 1;
    }

    add_data(samples);
}

}
