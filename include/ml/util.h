#pragma once

#include <vector>
#include <complex>

namespace ml {

struct array {

    array(std::vector<float> &ref);

    float *data;
    const unsigned size;

    // literally hitler, but copying the data around
    // is so stupid when this works aswell. It's not like
    // this code is going to run on any other machine than my laptop.
    std::complex<float> *cdata;
    const unsigned csize;
};

float random_float();

float sign(float);

}

