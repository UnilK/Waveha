#pragma once

#include <valarray>
#include <vector>
#include <complex>

namespace designf {

class Enveloper {

public:

    Enveloper(int minWindowSize, int maxWindowSize);

    float process(float sample);

    int get_delay();

private:

    int pointer, size, msize, dsize;
    std::valarray<float> ibuff, obuff;
  
    std::vector<int> wsize, state;
    std::vector<std::vector<float> > window;
    std::vector<std::vector<std::complex<float> > > wavelet;

};

}

