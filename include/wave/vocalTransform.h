#pragma once

#include "wave/vocal.h"

#include <cstdint>
#include <vector>
#include <complex>

namespace wave{

class VocalTransform{

protected:

    int32_t size = 90;
    float fundShift = 1.0f;
    std::vector<std::vector<std::complex<float> > > trans;

public:

    VocalTransform();
    VocalTransform(int32_t size_);

    void set_size(int32_t size_);
    int32_t get_size();

    void set_fundamental_shift(float fundShift_);
    float get_fundamental_shift();

    void set_value(int32_t from, int32_t to, std::complex<float> value);
    void set_transform(std::vector<std::vector<std::complex<float> > > trans_);

    Vocal transform(Vocal source);

};

}

