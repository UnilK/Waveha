#pragma once

#include "wave/buffer.h"
#include "wave/source.h"

#include <SFML/Audio/SoundStream.hpp>

#include <mutex>

namespace wave {

/*
    Note:

    openAL seems to use some HRTF by default.
    It's not a fit for my head and screws up the
    sounds.

    setting
    stereo-mode = speakers
    (etc)/openal/alsoft.conf

    fixed the thing (obviously a HRTF shouldn't be used with spwakers).
*/

class Player : public sf::SoundStream {
    
    // class for playing audio that has already been recorded.

public:

    Player(Source*);
    virtual ~Player();

    void open(Source*);

    // pushes 1/2 a second of audio at a time.
    bool onGetData(sf::SoundStream::Chunk &data);
    void onSeek(sf::Time timeOffset);

    void set_block(double);

    void lock();
    void unlock();

private:

    double blockSeconds = 0.5f;

    std::mutex sourceLock;

    Source *source;
    std::vector<sf::Int16> temp;

};



class Playback : public sf::SoundStream, public Buffer {

    // class for instant playback.

public:

    Playback(unsigned channels = 1, unsigned frameRate = 44100);
    virtual ~Playback();

    // wont stop unless explicitly told so.
    bool onGetData(sf::SoundStream::Chunk &data);
    
    // does nothing since this class handles near-instant playback.
    void onSeek(sf::Time timeOffset);

private:

    std::vector<sf::Int16> temp;

};

}
