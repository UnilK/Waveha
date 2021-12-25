#pragma once

#include "wave/readableAudio.h"

#include "wstream/wstream.h"

namespace wave {

class AudioFile : public ReadableAudio {

    // class for reading audio straight from a file

public:

    AudioFile();
    AudioFile(std::string fileName);

    bool open(std::string fileName);

    void onSeek(sf::Time timeOffset);
    void seek_sample(uint32_t sample);

    void refill();

protected:

    std::string name;
    iwstream file;

};

struct FixedAudioBuffer {

    // simple struct for handling audio data

    FixedAudioBuffer();
    FixedAudioBuffer(const std::vector<float> &audio_,
            uint32_t channels_ = 1, uint32_t frameRate_ = 44100);
    FixedAudioBuffer(std::string fileName);

    std::vector<float> audio;
    uint32_t channels = 1, frameRate = 44100;
    
};

class LoadedAudioFile : public ReadableAudio {
    
    // class for reading audio from a cached buffer

public:

    LoadedAudioFile();
    LoadedAudioFile(FixedAudioBuffer *buff_);

    bool open(FixedAudioBuffer *buff_);

    void onSeek(sf::Time timeOffset);
    void seek_sample(uint32_t sample);

    void refill();

protected:

    FixedAudioBuffer *buff = nullptr;
    uint32_t position = 0;

};

}

