#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <mutex>

namespace wave {

class Source {

public:

    Source();
    virtual ~Source();

    // seek sample for internal pointer.
    virtual void seek(uint32_t sample) = 0;
    
    // get amount samples starting from an internal pointer and move it.
    // vector is padded with zeros if no real samples are found.
    // returns 0 if padding was needed, otherwise 1.
    virtual bool pull(uint32_t amount, std::vector<float> &samples) = 0;
    
    // get amount samples starting from sample begin.
    // vector is padded with zeros if no real samples are found.
    virtual std::vector<float> get(uint32_t amount, uint32_t begin) = 0;

    std::string name;

    // use for access only
    uint32_t channels, frameRate;

    bool good = 1;

};

}

