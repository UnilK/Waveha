#pragma once

#include <complex>
#include <valarray>
#include <vector>

namespace designb {

class Splitter {

public:

    Splitter(int size);

    std::valarray<float> process(float sample);

    double middle_period(int band);

    int get_size();

    int get_delay();

private:

    int band_left(int band);
    int band_right(int band);

    int bsize, wsize, size, pointer, state = 0;
    std::valarray<float> ibuff;
    std::valarray<std::valarray<float> > obuff;

    std::vector<float> window;
    std::vector<std::vector<std::complex<float> > > wavelet;
    
    std::valarray<int> ctype;
};

}

