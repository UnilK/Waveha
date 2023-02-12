#pragma once

#include <vector>
#include <complex>

namespace designa {

class Ftip {

    // pitch shifter

public:

    Ftip(int size);

    std::vector<float> process(float sample);

    void set_shift(float pitch_shift);

    int get_delay();

private:

    float shift, out;
    int wsize, osize, in, done, state;
    std::vector<float> ibuff, obuff;
    std::vector<std::vector<std::complex<float> > > wavelet;

};

}

