#pragma once

#include <cstdint>
#include <vector>

namespace wave{
   
bool is_noise(
        std::vector<float> &waves,
        float tolerance);
bool is_noise(
        float *waves,
        int32_t size,
        float tolerance);

bool is_silent(
        std::vector<float> &waves,
        float tolerance);
bool is_silent(
        float *waves,
        int32_t size,
        float tolerance);

}

