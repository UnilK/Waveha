#pragma once

#include <vector>
#include <complex>

namespace change {

class Pitcher2 {

    // pitch shifter

public:

    Pitcher2(int size);

    std::vector<float> process(float sample);

    void set_shift(double pitch_shift);

    int get_delay();

private:

    double shift, out;
    int wsize, osize, in, done, state;
    std::vector<float> ibuff, obuff;
    std::vector<std::vector<std::complex<float> > > wavelet;

};

}

