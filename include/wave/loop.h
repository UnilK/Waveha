#pragma once

#include "wave/source.h"

namespace wave {

class Loop : public Source {

public:

    Loop(Source&);
    virtual ~Loop();
   
    void open(Source&);

    void seek(unsigned sample);
    unsigned tell();
    
    // does not have a size. Returns 0.
    unsigned size();

    unsigned pull(unsigned amount, std::vector<float> &samples);
    std::vector<float> get(unsigned amount, unsigned begin);

protected:
   
    Source &source;
    unsigned read = 0;

};

}

