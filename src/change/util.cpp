#include "change/util.h"

#include <algorithm>
#include <cstring>

namespace change {

std::mt19937 rng32;
double rnd(){ return std::uniform_real_distribution<double>(0, 1)(rng32); }

PID::PID(float p_, float i_, float d_, float dt_) : p(p_), i(i_), d(d_) {
    dt = dt_;
    y = 0.0f;
    ie = e = de = pe = 0.0f;
}

void PID::initialize(float y_){
    y = y_;
    ie = e = de = pe = 0.0f;
}

float PID::get(float target){

    pe = e;
    e = target - y;
    ie += e * dt;
    de = (e - pe) / dt;

    y += p * e + i * ie + d * de;
    
    return y;
}

void rvec::init(int size_, int index_, float val){
    size = size_;
    buffer = 8;
    length = 2 * buffer + 8 * size;
    index = std::min(length - buffer - size - 1, std::max(buffer, index_ + buffer));
    if(data != nullptr) delete[] data;
    data = new(std::align_val_t(32)) float[length];
    for(int i=0; i<length; i++) data[i] = val;
}

rvec::~rvec(){
    delete[] data;
}

void rvec::push(const float &x){
    
    index++;
    if(index + size + buffer >= length){
        memmove(data, data + index - buffer, (size + buffer) * sizeof(float));
        index = buffer;
    }

    data[index + size - 1] = x;
}

ticker::ticker(int period_, int speed_, int index_) :
    period(period_),
    speed(speed_),
    index(index_),
    sindex(0)
{}

int ticker::tick(){
    sindex = (sindex + 1) % speed;
    if(sindex == 0){
        index = (index + 1) % period;
        return index;
    }
    return -1;
}

}

