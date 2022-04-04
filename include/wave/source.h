#pragma once

#include <vector>
#include <string>

namespace wave {

class Source {

public:

    Source();
    virtual ~Source();

    // seek sample for internal pointer.
    virtual void seek(unsigned sample) = 0;
    
    // tell the position of the internal pointer
    virtual unsigned tell() = 0;
    
    // size of source in samples
    virtual unsigned size() = 0;

    // get amount samples starting from an internal pointer and move it.
    // vector is padded with zeros if no real samples are found.
    // returns the amount of actual samples read.
    // use for playing audio.
    virtual unsigned pull(unsigned amount, std::vector<float> &samples) = 0;
    
    // get amount samples starting from sample begin.
    // vector is padded with zeros if no real samples are found.
    // use for accessing data.
    virtual std::vector<float> get(unsigned amount, unsigned begin) = 0;

    unsigned pull_loop(unsigned amount, std::vector<float> &samples);
    std::vector<float> get_loop(unsigned amount, unsigned begin);

    std::string name;

    // use for access only
    unsigned channels, frameRate;

    bool good = 1;

};

}

