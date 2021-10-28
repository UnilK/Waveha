#pragma once

#include <cstdint>
#include <complex>
#include <vector>

namespace math{

// extract frequency from waves
std::complex<float> roll(std::vector<float> &waves, int32_t size, float frequency);

}

