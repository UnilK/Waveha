#include "wave/cache.h"
#include "wstream/wstream.h"

#include <string.h>
#include <cassert>

namespace wave {


Cache::Cache(Audio *audio_){
    open(audio_);
}

Cache::~Cache(){}

void Cache::open(Audio *audio_){
    assert(audio_ != nullptr);
    audio = audio_;
    channels = audio->channels;
    frameRate = audio->frameRate;
    seek(0);
}

void Cache::seek(unsigned sample){
    if(audio->data.size() > 0)
        read = std::min(sample, (unsigned)audio->data.size()-1);
}

unsigned Cache::tell(){
    return read;
}

unsigned Cache::size(){
    return audio->data.size();
}

unsigned Cache::pull(unsigned amount, std::vector<float> &samples){

    good = 1;

    unsigned begin = samples.size();

    samples.resize(samples.size() + amount, 0.0f);

    unsigned actual = std::min(amount, (unsigned)audio->data.size() - read);

    if(amount != samples.size()) good = 0;

    memcpy(samples.data() + begin, audio->data.data() + read, sizeof(float) * actual);
    read += amount;

    return actual;
}

std::vector<float> Cache::get(unsigned amount, unsigned begin){
    
    std::vector<float> samples(amount, 0.0f);

    if(begin < audio->data.size()) amount = std::min(amount, (unsigned)audio->data.size() - begin);
    else amount = 0;

    memcpy(samples.data(), audio->data.data() + begin, sizeof(float) * amount);

    return samples;
}

}
