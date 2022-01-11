#pragma once

#include "wave/source.h"

namespace wave {

class Cache : public Source {

public:

    Cache();

    Cache(const std::vector<float> &audio_,
            uint32_t channels_ = 1,
            uint32_t frameRate_ = 44100);
    
    Cache(std::string fileName);
    
    void open(const std::vector<float> &audio_,
            uint32_t channels_ = 1,
            uint32_t frameRate_ = 44100);
    
    void open(std::string fileName);

    void seek(uint32_t sample);

    std::vector<float> pull(uint32_t amount);
    std::vector<float> get(uint32_t amount, uint32_t begin);

protected:
    
    std::vector<float> audio;
    uint32_t read = 0;

};

}

