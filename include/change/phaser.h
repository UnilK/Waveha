#pragma once

#include <vector>
#include <complex>

namespace change {

class Phaser {

public:

    Phaser(int rate, float low, float high);

    float process(float sample);

    void set_shift(float pitch_shift);

    float get_scale();
    float get_similarity();

    int delay();

    std::vector<float> debug();

private:

    void find_scale();
    void check_scale();

    float out, shift, scale, similarity;

    int min, max, mid, in, done, wstate, fstate, owlen, flen, padding;
    std::vector<float> ibuff, obuff, window, mse;

    std::vector<std::vector<std::complex<float> > > wavelet;

    const int wlen = 8;

};

}

