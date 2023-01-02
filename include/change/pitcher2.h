#pragma once

#include <vector>
#include <complex>

namespace change {

class Pitcher2 {

    // pitch shifter

public:

    Pitcher2(double shift_, int size_);

    std::vector<float> process(float sample);

    int delay();

private:

    double shift, step, out;
    int size, ssize, isize, osize, in, done, phase, period, freqs;
    std::vector<float> ibuff, obuff, window;
    std::vector<std::vector<std::complex<float> > > w;

};

}

