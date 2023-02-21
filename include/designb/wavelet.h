#pragma once

#include <complex>
#include <deque>
#include <valarray>
#include <vector>
#include <tuple>

namespace designb {

class Wavelet {

public:

    Wavelet(double length, double spins);

    std::vector<float> process(float sample);

    void set_shift(double);
    void set_color_shift(double);
    void set_decay(float);

    int get_delay();

private:

    std::complex<float> eval(float, float);

    double win, out, wsize, spin, shift, cshift;

    float decay, energy;
    std::complex<float> phase;

    int in, size, iwsize;
    std::valarray<float> buffer;
    std::deque<std::tuple<double, std::complex<float> > > wavelets;

};

}

