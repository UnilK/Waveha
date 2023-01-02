#pragma once

#include <vector>

namespace math {

struct DCOSPrecalc {
    DCOSPrecalc();
    static const unsigned N = 1<<10;
    std::vector<float > cosw;
};

extern DCOSPrecalc dcosPrecalc;

float dcos(float x);

}

