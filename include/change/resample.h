#pragma once

#include <vector>

namespace change {

// resamples audio so that pitch is multiplied by shift. default window size is 16.
std::vector<float> resample(
        const std::vector<float> &audio,
        double shift,
        int size = 16);

}

