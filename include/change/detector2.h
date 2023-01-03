#pragma once

#include <vector>
#include <complex>

namespace change {

class Detector2 {

public:

    Detector2(int rate_, float low_, float high = 1500.0f, int points = 256);

    void push(float sample);
    
    float pitch();
    int period();

    std::vector<float> debug();

private:

    float step, low, f0;
    int rate, size;
    std::vector<std::complex<float> > root, echo;
    std::vector<float> energy, dbg;

};

}

