#pragma once

#include <string>
#include <vector>

namespace wave {

struct Audio {

    Audio();
    
    Audio(    
        std::string name_,
        unsigned channels_,
        unsigned frameRate_,
        const std::vector<float> &data_);

    Audio(const Audio &other);

    Audio(std::string fileName);
    
    virtual ~Audio();

    bool open(std::string fileName);
    
    std::string name;
    unsigned channels = 1, frameRate = 44100;
    std::vector<float> data;

};

}

