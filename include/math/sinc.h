#pragma once

#include <vector>

namespace math {

std::vector<float> resample(
        const std::vector<float> &in,
        double factor,
        int zeros = 16);

struct SINCPrecalc {
    SINCPrecalc();
    static const unsigned ZERO = 128;
    static const unsigned N = 1<<10;
    static constexpr unsigned ZN = ZERO*N;
    std::vector<float > fsinc;
};

extern SINCPrecalc sincPrecalc;

// returns sin(pi*x) / (pi*x) up to ZERO zero crossings and 0 after that.
// x is expected to be non-negative. large inputs will be undefined.
float sinc(float x);
float sinc(double x);

}

