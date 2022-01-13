#pragma once

#include "wave/buffer.h"
#include "wave/source.h"

#include <SFML/Audio/SoundStream.hpp>

namespace wave {

    // class for playing audio that has already been recorded.

class Player : public sf::SoundStream {

public:

    Player(Source &source_);
    virtual ~Player();

    // pushes 1/2 a second of audio at a time.
    bool onGetData(sf::SoundStream::Chunk &data);
    void onSeek(sf::Time timeOffset);

private:

    Source &source;
    std::vector<sf::Int16> temp;

};



class Playback : public sf::SoundStream, public Buffer {

    // class for instant playback.

public:

    Playback(uint32_t channels = 1, uint32_t frameRate = 44100);
    virtual ~Playback();

    // wont stop unless explicitly told so.
    bool onGetData(sf::SoundStream::Chunk &data);
    
    // does nothing since this class handles near-instant playback.
    void onSeek(sf::Time timeOffset);

private:

    std::vector<sf::Int16> temp;

};

}
