#ifndef FT_H
#define FT_H

#include <cstdint>
#include <complex>
#include <vector>

namespace math{

// extract frequency from waves
std::complex<float> roll(std::vector<float> &waves, int32_t size, float frequency);

};

#endif
