#include "wave/cache.h"
#include "wstream/wstream.h"

#include <string.h>

namespace wave {

Cache::Cache(const std::vector<float> &audio_, uint32_t channels_, uint32_t frameRate_){
    open(audio_, channels_, frameRate_);
}

Cache::Cache(std::string fileName){
    open(fileName);
}

void Cache::open(const std::vector<float> &audio_, uint32_t channels_, uint32_t frameRate_){

    good = 1;

    audio = audio_;
    channels = channels_;
    frameRate = frameRate_;

}

void Cache::open(std::string fileName){
    
    good = 1;

    iwstream file;

    if(file.open(fileName)){
        frameRate = file.get_frame_rate();
        channels = file.get_channel_amount();
        audio = file.read_file();
    }
    else good = 0;
}

void Cache::seek(uint32_t sample){
    read = std::min(sample, (uint32_t)audio.size());
}

std::vector<float> Cache::pull(uint32_t amount){
    
    good = 1;

    std::vector<float> samples(amount, 0.0f);

    amount = std::min(amount, (uint32_t)audio.size() - read);

    if(amount != samples.size()) good = 0;

    memcpy(samples.data(), audio.data() + read, sizeof(float) * amount);
    read += amount;

    return samples;
}

std::vector<float> Cache::get(uint32_t amount, uint32_t begin){
    
    std::vector<float> samples(amount, 0.0f);

    if(begin < audio.size()) amount = std::min(amount, (uint32_t)audio.size() - begin);
    else amount = 0;

    memcpy(samples.data(), audio.data() + begin, sizeof(float) * amount);

    return samples;
}

}
