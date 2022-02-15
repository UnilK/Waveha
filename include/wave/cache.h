#pragma once

#include "wave/source.h"
#include "wave/audio.h"

namespace wave {

class Cache : public Source {

public:

    Cache(Audio*);
    virtual ~Cache();
   
    void open(Audio*);

    void seek(unsigned sample);
    unsigned tell();
    unsigned size();

    unsigned pull(unsigned amount, std::vector<float> &samples);
    std::vector<float> get(unsigned amount, unsigned begin);

protected:
   
    Audio *audio;
    unsigned read = 0;

};

}

