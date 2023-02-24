#pragma once

#include <valarray>
#include <vector>
#include <complex>

namespace designc {

class Equalizer {

public:

    Equalizer(int windowSize);

    float process(float sampler);

    void enable_gain(bool);
    void enable_clean(bool);
    void enable_noise(bool);
    void enable_shuffle(bool);
    void enable_blur(bool);

    void set_gain(const std::vector<float> &new_gain);
    void set_clean(const std::vector<float> &new_gain);
    void set_noise(const std::vector<float> &new_noise);
    void set_shuffle(const std::vector<float> &new_shuffle);
    void set_blur(const std::vector<float> &new_shuffle);

    int get_frequency_size();
    int get_delay();

private:

    void apply_gain(std::vector<std::complex<float> > &freq);
    void apply_clean(std::vector<std::complex<float> > &freq);
    void apply_noise(std::vector<std::complex<float> > &freq);
    void apply_shuffle(std::vector<std::complex<float> > &freq);
    void apply_blur(std::vector<std::complex<float> > &freq);

    int wsize, size, fsize, pointer, state;
    std::valarray<float> ibuff, obuff;

    bool gain_x, clean_x, noise_x, shuffle_x, blur_x;
    std::vector<float> window, gain, clean, noise, shuffle, blur;

};

}

