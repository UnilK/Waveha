#include "wave/buffer.h"
#include "wave/util.h"

#include <algorithm>
#include <string.h>

namespace wave {

Buffer::Buffer(unsigned capacity_) : capacity(capacity_) {}

Buffer::~Buffer(){}

std::vector<float> Buffer::pull(unsigned amount){
   
    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    std::vector<float> data(amount, 0.0f);
    
    amount = std::min(amount, write - read);
    memcpy(data.data(), buffer.data() + read, sizeof(float) * amount);
    read += amount;

    if(read * 2 > buffer.size()){
        write -= read;
        memmove(buffer.data(), buffer.data() + read, sizeof(float) * write);
        read = 0;
    }

    return data;
}

void Buffer::push(const float *data, unsigned amount){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);

    buffer.resize(std::max(write + amount, (unsigned)buffer.size()));

    memcpy(buffer.data() + write, data, sizeof(float) * amount);
    write += amount;
}

void Buffer::push(const std::vector<float> &data){
    push(data.data(), data.size());
}

unsigned Buffer::max(){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    unsigned max = write - read;
    
    return max;
}

unsigned Buffer::hunger(){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    if(capacity < (write - read)) return 0;
    unsigned hunger = capacity - (write - read);
    
    return hunger;
}

void Buffer::reset(){

    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    buffer.clear();
    read = 0;
    write = 0;
}

}
