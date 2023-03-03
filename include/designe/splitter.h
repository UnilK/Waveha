#pragma once

#include <valarray>
#include <vector>
#include <complex>

namespace designe {

struct SplitSample {
    float raw, low, high, energy;
};

class Splitter {

public:

    Splitter(int windowSize);

    SplitSample process(float sample);

    int get_delay();

private:

    int pointer, size, wsize, fsize, state;
    std::valarray<float> ibuff, lbuff, ebuff;
    std::vector<float> window;
    std::vector<std::complex<float> > wavelet;

};

}

