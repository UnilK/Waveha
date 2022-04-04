#include "wave/sound.h"
#include "wave/util.h"

#include <algorithm>
#include <math.h>
#include <cassert>

namespace wave {

Player::Player(Source *source_) {
    open(source_);
}

Player::~Player(){}

void Player::open(Source *s){
    assert(s != nullptr);
    source = s;
    initialize(source->channels, source->frameRate);
}

void Player::set_block(double d){
    blockSeconds = d;
}

void Player::set_loop(bool b){
    looping = b;
}

bool Player::onGetData(sf::SoundStream::Chunk &data){

    sourceLock.lock();

    unsigned amount = (unsigned)std::round(getSampleRate() * getChannelCount() * blockSeconds);
    
    std::vector<float> floats;
    
    unsigned actual;
    if(looping) actual = source->pull_loop(amount, floats);
    else actual = source->pull(amount, floats);
    
    sourceLock.unlock();

    temp = float_to_int(floats);
    
    data.sampleCount = actual;
    data.samples = temp.data();

    return amount == actual;
}

void Player::onSeek(sf::Time timeOffset){
    sourceLock.lock();
    source->seek(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
    sourceLock.unlock();
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
