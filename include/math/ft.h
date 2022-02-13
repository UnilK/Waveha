#pragma once

#include <complex>
#include <vector>

namespace math {

using std::vector;
using std::complex;

complex<float> extract_frequency(float *waves, unsigned size, float frequency);
complex<float> extract_frequency(vector<float> &waves, float frequency);

struct FTPrecalc {
    FTPrecalc();
    static const unsigned N = 1<<17;
    complex<float> exp[N];
};

extern FTPrecalc ftPrecalc;

}

