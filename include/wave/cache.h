#pragma once

#include "wave/source.h"

namespace wave {

struct Audio {

    Audio();
    Audio(std::string fileName);
    virtual ~Audio();

    bool open(std::string fileName);
    
    std::string name;
    std::vector<float> data;
    uint32_t channels, frameRate;

};

class Cache : public Source {

public:

    Cache();
    Cache(Audio&);
    virtual ~Cache();
   
    void open(Audio&);

    void seek(uint32_t sample);

    bool pull(uint32_t amount, std::vector<float> &samples);
    std::vector<float> get(uint32_t amount, uint32_t begin);

protected:
   
    Audio &audio;
    uint32_t read = 0;

};

}

