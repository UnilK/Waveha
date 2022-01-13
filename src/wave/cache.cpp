#include "wave/cache.h"
#include "wstream/wstream.h"

#include <string.h>

namespace wave {

Audio::Audio(){}

Audio::Audio(std::string fileName){
    open(fileName);
}

Audio::~Audio(){}

bool Audio::open(std::string fileName){

    iwstream file;

    if(file.open(fileName)){
        
        name = fileName;

        frameRate = file.get_frame_rate();
        channels = file.get_channel_amount();
        
        data = file.read_file();
        
        return 1;
    }

    return 0;
}



Cache::Cache(Audio &audio_) : audio(audio_) {
    channels = audio.channels;
    frameRate = audio.frameRate;
}

Cache::~Cache(){}

void Cache::open(Audio &audio_){
    audio = audio_;
    channels = audio.channels;
    frameRate = audio.frameRate;
}

void Cache::seek(uint32_t sample){
    read = std::min(sample, (uint32_t)audio.data.size());
}

bool Cache::pull(uint32_t amount, std::vector<float> &samples){

    good = 1;

    samples = std::vector<float>(amount, 0.0f);

    amount = std::min(amount, (uint32_t)audio.data.size() - read);

    if(amount != samples.size()) good = 0;

    memcpy(samples.data(), audio.data.data() + read, sizeof(float) * amount);
    read += amount;

    return good;
}

std::vector<float> Cache::get(uint32_t amount, uint32_t begin){
    
    std::vector<float> samples(amount, 0.0f);

    if(begin < audio.data.size()) amount = std::min(amount, (uint32_t)audio.data.size() - begin);
    else amount = 0;

    memcpy(samples.data(), audio.data.data() + begin, sizeof(float) * amount);

    return samples;
}

}
