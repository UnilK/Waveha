#include "wave/buffer.h"
#include "wave/util.h"

#include <algorithm>
#include <string.h>

namespace wave {

Buffer::Buffer(uint32_t capacity_) : capacity(capacity_) {}

Buffer::~Buffer(){}

std::vector<float> Buffer::pull(uint32_t amount){
   
    lock.lock();
    
    std::vector<float> data(amount, 0.0f);
    
    amount = std::min(amount, write - read);
    memcpy(data.data(), buffer.data() + read, sizeof(float) * amount);
    read += amount;

    if(read * 2 > buffer.size()){
        write -= read;
        memmove(buffer.data(), buffer.data() + read, sizeof(float) * write);
        read = 0;
    }
    
    lock.unlock();

    return data;
}

void Buffer::push(float *data, uint32_t amount){
    
    lock.lock();

    buffer.resize(std::max(write + amount, (uint32_t)buffer.size()));

    memcpy(buffer.data() + write, data, sizeof(float) * amount);
    write += amount;

    lock.unlock();
}

void Buffer::push(std::vector<float> &data){
    push(data.data(), data.size());
}

uint32_t Buffer::max(){
    
    lock.lock();
    uint32_t max = write - read;
    lock.unlock();
    
    return max;
}

uint32_t Buffer::hunger(){
    
    lock.lock();
    if(capacity < (write - read)) return 0;
    uint32_t hunger = capacity - (write - read);
    lock.unlock();
    
    return hunger;
}

void Buffer::reset(){

    lock.lock();
    
    buffer.clear();
    read = 0;
    write = 0;

    lock.unlock();
}

}
