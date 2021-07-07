#ifndef FT_H
#define FT_H

#include <cstdint>
#include <complex>

namespace math{

    // extract frequency from waves
    std::complex<float> roll(float *waves, int32_t size, float frequency);

};

#endif
