#pragma once

#include <vector>
#include <complex>

namespace ml {

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
    return 2.0f * (float)rand() / RAND_MAX - 1.0f;
}

inline float sign(float x){
    return (x>0.0f)-(x<0.0f);
}

}

