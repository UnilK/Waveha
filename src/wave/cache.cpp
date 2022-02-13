#include "wave/cache.h"
#include "wstream/wstream.h"

#include <string.h>

namespace wave {


Cache::Cache(Audio &audio_) : audio(audio_) {
    open(audio_);
}

Cache::~Cache(){}

void Cache::open(Audio &audio_){
    audio = audio_;
    channels = audio.channels;
    frameRate = audio.frameRate;
    seek(0);
}

void Cache::seek(unsigned sample){
    read = std::min(sample, (unsigned)audio.data.size());
}

unsigned Cache::tell(){
    return read;
}

unsigned Cache::size(){
    return audio.data.size();
}

unsigned Cache::pull(unsigned amount, std::vector<float> &samples){

    good = 1;

    samples = std::vector<float>(amount, 0.0f);

    amount = std::min(amount, (unsigned)audio.data.size() - read);

    if(amount != samples.size()) good = 0;

    memcpy(samples.data(), audio.data.data() + read, sizeof(float) * amount);
    read += amount;

    return amount;
}

std::vector<float> Cache::get(unsigned amount, unsigned begin){
    
    std::vector<float> samples(amount, 0.0f);

    if(begin < audio.data.size()) amount = std::min(amount, (unsigned)audio.data.size() - begin);
    else amount = 0;

    memcpy(samples.data(), audio.data.data() + begin, sizeof(float) * amount);

    return samples;
}

}
