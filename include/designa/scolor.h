#pragma once

#include <valarray>
#include <vector>

namespace designa {

class Scolor {

public:

    Scolor(int framerate, float min_frequency, float decay_halftime, int window_steps = 4);

    float process(float sample, float period);

    int get_shift_size();
    
    void set_shift(const std::vector<float> &new_shift);

    int get_delay();

private:

    float decay;
    int pointer, size, wsize, psize, state, step;
    std::valarray<float> ibuff, obuff, pbuff;
    std::vector<float> shift;

};

}

