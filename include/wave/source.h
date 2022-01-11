#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace wave {

class Source {

public:

    Source();
    virtual ~Source();

    virtual void seek(uint32_t sample);
    virtual std::vector<float> pull(uint32_t amount);
    virtual std::vector<float> get(uint32_t amount, uint32_t begin);

    std::string name;

    // use for access only
    uint32_t channels, frameRate;
    bool good = 1;

};

}

