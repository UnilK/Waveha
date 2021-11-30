#pragma once

#include "wstream/wstream.h"

#include <SFML/Audio/SoundStream.hpp>

#include <string>
#include <mutex>

namespace wave {

class AudioFile : public sf::SoundStream {

public:

    AudioFile(std::string fileName);

    bool bad();

    bool onGetData(Chunk &data);

    void onSeek(sf::Time timeOffset);

protected:

    std::string name;
    iwstream file;

    std::mutex read_lock;

    bool badBit = 0;
    
};

}

