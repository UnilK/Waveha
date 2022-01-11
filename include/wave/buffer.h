#pragma once

#include <vector>
#include <mutex>

namespace wave {

class Buffer{

public:

    Buffer(uint32_t capacity_ = 1<<15);
    virtual ~Buffer();
    
    // retrieve data
    std::vector<float> pull(uint32_t amount);

    // append data
    void push(float *data, uint32_t amount);
    void push(std::vector<float> &data);

    // how much data the buffer needs until it is satisfied?
    uint32_t hunger();
    
    // the amount of available data
    uint32_t max();
    
protected:
    
    // number of samples needed to satisfy the buffer
    uint32_t capacity;

    // clear all data and reset pointers.
    void reset();
    
    std::vector<float> buffer;
    std::mutex lock;

    uint32_t read = 0;
    uint32_t write = 0;

};

}
