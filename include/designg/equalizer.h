#pragma once

#include <valarray>
#include <vector>
#include <complex>

namespace designg {

class Equalizer {

public:

    Equalizer(int windowSize);

    float process(float sampler);

    void enable_gain(bool);
    void enable_clean(bool);

    void set_gain(const std::vector<float> &new_gain);
    void set_clean(const std::vector<float> &new_gain);

    int get_frequency_size();
    int get_delay();

private:

    void apply_gain(std::vector<std::complex<float> > &freq);
    void apply_clean(std::vector<std::complex<float> > &freq);

    int wsize, size, fsize, pointer, state;
    std::valarray<float> ibuff, obuff;

    bool gain_x, clean_x;
    std::vector<float> window, gain, clean;

};

}

