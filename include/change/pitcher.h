#pragma once

#include <vector>

namespace change {

class Pitcher {

    // pitch shifter

public:

    Pitcher(double shift_, int zeros_);

    std::vector<float> process(float sample);

    int delay();

private:

    double shift, in, wsize, step, istep;
    int zeros, bsize, wisize, out;
    std::vector<float> buffer;

};

}

