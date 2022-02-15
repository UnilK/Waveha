#include "wave/sound.h"
#include "wave/util.h"

#include <algorithm>
#include <math.h>

namespace wave {

Player::Player(Source *source_) {
    open(source_);
}

Player::~Player(){}

void Player::open(Source *s){
    source = s;
    initialize(source->channels, source->frameRate);
}

void Player::set_block(double d){
    blockSeconds = d;
}

bool Player::onGetData(sf::SoundStream::Chunk &data){

    unsigned amount = (unsigned)std::round(getSampleRate() * getChannelCount() * blockSeconds);
    
    std::vector<float> floats;
    unsigned actual = source->pull(amount, floats);

    temp = float_to_int(floats);
    
    data.sampleCount = actual;
    data.samples = temp.data();

    return amount == actual;
}

void Player::onSeek(sf::Time timeOffset){
    source->seek(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
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
