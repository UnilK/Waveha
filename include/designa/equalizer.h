#pragma once

#include <valarray>
#include <vector>
#include <complex>

namespace designa {

class Equalizer {

public:

    Equalizer(int window_size);

    float process(float processed, float raw);

    void enable_merge(bool);
    void enable_gain(bool);
    void enable_clean(bool);
    void enable_noise(bool);
    void enable_shuffle(bool);

    void set_merge(const std::vector<float> &new_merge);
    void set_gain(const std::vector<float> &new_gain);
    void set_clean(const std::vector<float> &new_gain);
    void set_noise(const std::vector<float> &new_noise);
    void set_shuffle(const std::vector<float> &new_shuffle);

    int get_frequency_size();
    int get_delay();

private:

    void apply_merge(
            std::vector<std::complex<float> > &pfreq,
            std::vector<std::complex<float> > &rfreq);
    
    void apply_gain(std::vector<std::complex<float> > &freq);
    void apply_clean(std::vector<std::complex<float> > &freq);
    void apply_noise(std::vector<std::complex<float> > &freq);
    void apply_shuffle(std::vector<std::complex<float> > &freq);

    int wsize, size, fsize, pointer, state;
    std::valarray<float> pbuff, rbuff, obuff;

    bool merge_x, gain_x, clean_x, noise_x, shuffle_x;
    std::vector<float> window, merge, gain, clean, noise, shuffle;

};

}

