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

    void setChannel(int32_t);

    // check out the file contents without changing the file reading position
    // If there are multiple channels, then "amount" samples are read from
    // the currently selected channel.
    std::vector<float> peekData(uint32_t amount);
    std::vector<float> lookData(uint32_t position, uint32_t amount);

protected:

    std::string name;
    iwstream file;

    std::mutex readLock;

    bool badBit = 0;
    uint16_t channels = 1, channel = 0;
    uint32_t frameRate = 1;
    
    std::vector<sf::Int16> converted;  
    
    void parse_channel(std::vector<float> &data);

};

}

