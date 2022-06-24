#pragma once

#include <vector>
#include <complex>
#include <random>
#include <chrono>

namespace ml {

static std::mt19937 rng32(std::chrono::steady_clock::now().time_since_epoch().count());

struct array {

    array(std::vector<float> &ref) :
        data(ref.data()),
        size(ref.size()),
        cdata((std::complex<float>*)ref.data()),
        csize(ref.size()/2)
    {}

    float *data;
    const unsigned size;

    // literally hitler, but copying the data around
    // is so stupid when this works aswell. It's not like
    // this code is going to run on any other machine than my laptop.
    std::complex<float> *cdata;
    const unsigned csize;

    float &operator[](unsigned i){ return data[i]; }
    std::complex<float> &operator()(unsigned i){ return cdata[i]; }

};


inline float random_float(){ 
    return (float)(int)rng32() / (1ll<<31);
}

inline float sign(float x){
    return (x>0.0f)-(x<0.0f);
}

}

