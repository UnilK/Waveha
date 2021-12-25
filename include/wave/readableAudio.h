#pragma once

#include "wave/audioBuffer.h"

#include <SFML/Audio/SoundStream.hpp>

namespace wave{

class ReadableAudio : public AudioBuffer, public sf::SoundStream {

public:

    using AudioBuffer::AudioBuffer;

    // executed when actually playing sounds.
    bool onGetData(Chunk &data);

    // scrolling is done by samples for visual inspection purposes.
    virtual void onSeek(sf::Time timeOffset);
    virtual void seek_sample(uint32_t sample);

    // function to refill the buffer.
    virtual void refill();

    // reading modes used with onGetData.
    // 0 -> read everything from buffer
    // 1 -> read half from buffer
    // 2 -> read fixed amount from buffer
    // TODO : turn this into an enum
    uint32_t readMode = 0;
    
    // how many samples are to be played per call if mode = 2?
    uint32_t dataBlock = 1<<10;

    bool good();

protected:

    // if nothing good can be loaded on to buffer then it should be signaled with these.
    uint32_t goodSamples = 0;
    bool shouldStop = 0, goodBit = 0;

};

}

