#pragma once

#include <vector>
#include <mutex>

namespace wave {

class Buffer {

public:

    Buffer(unsigned capacity_ = 1<<15);
    virtual ~Buffer();
    
    // retrieve data
    std::vector<float> pull(unsigned amount);

    // append data
    void push(const float *data, unsigned amount);
    void push(const std::vector<float> &data);

    // how much data the buffer needs until it is satisfied?
    unsigned hunger();
    
    // the amount of available data
    unsigned max();
    
protected:
    
    // number of samples needed to satisfy the buffer
    unsigned capacity;

    // clear all data and reset pointers.
    void reset();
    
    std::vector<float> buffer;
    std::mutex lock;

    unsigned read = 0;
    unsigned write = 0;

};

}
