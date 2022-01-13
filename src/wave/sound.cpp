#include "wave/sound.h"
#include "wave/util.h"

#include <algorithm>

namespace wave {

Player::Player(Source &source_) : source(source_) {
    initialize(source.channels, source.frameRate);
}

Player::~Player(){}

bool Player::onGetData(sf::SoundStream::Chunk &data){

    uint32_t amount = getSampleRate() * getChannelCount() / 2;
    
    std::vector<float> floats;
    bool ok = source.pull(amount, floats);

    temp = float_to_int(floats);
    
    data.sampleCount = amount;
    data.samples = temp.data();

    return ok;
}

void Player::onSeek(sf::Time timeOffset){
    source.seek(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
}



Playback::Playback(uint32_t channels, uint32_t frameRate){
    initialize(channels, frameRate);
}

Playback::~Playback(){}

bool Playback::onGetData(sf::SoundStream::Chunk &data){

    uint32_t amount = std::max(getChannelCount(), max() / 2 / getChannelCount() * getChannelCount());

    std::vector<float> floats = pull(amount);

    temp = float_to_int(floats);
    
    data.sampleCount = amount;
    data.samples = temp.data();

    return 1;
}

void Playback::onSeek(sf::Time timeOffset){}

}
