#pragma once

#include <vector>
#include <cstdint>

namespace change {

struct Pitch {

    float minF = 80, maxF = 800;

    int peaks = 10;
    float factor = 2;

    // vector length must be a power of 2.
    std::vector<float> graph(const std::vector<float> &audio);
    int period(const std::vector<float> &audio);

};



}

