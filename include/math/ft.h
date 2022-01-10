#pragma once

#include <cstdint>
#include <complex>
#include <vector>

namespace math{

using std::vector;
using std::complex;

complex<float> extract_frequrncy(float *waves, int32_t size, float frequency);
complex<float> extract_frequrncy(vector<float> &waves, float frequency);

}

