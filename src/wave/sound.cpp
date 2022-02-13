#include "wave/sound.h"
#include "wave/util.h"

#include <algorithm>

namespace wave {

Player::Player(Source &source_) : source(source_) {
    initialize(source.channels, source.frameRate);
}

Player::~Player(){}

bool Player::onGetData(sf::SoundStream::Chunk &data){

    unsigned amount = getSampleRate() * getChannelCount() / 2;
    
    std::vector<float> floats;
    unsigned actual = source.pull(amount, floats);

    temp = float_to_int(floats);
    
    data.sampleCount = actual;
    data.samples = temp.data();

    return amount == actual;
}

void Player::onSeek(sf::Time timeOffset){
    source.seek(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
}



Playback::Playback(unsigned channels, unsigned frameRate){
    initialize(channels, frameRate);
}

Playback::~Playback(){}

bool Playback::onGetData(sf::SoundStream::Chunk &data){

    unsigned amount = std::max(getChannelCount(), max() / 2 / getChannelCount() * getChannelCount());

    std::vector<float> floats = pull(amount);

    temp = float_to_int(floats);
    
    data.sampleCount = amount;
    data.samples = temp.data();

    return 1;
}

void Playback::onSeek(sf::Time timeOffset){}

}
