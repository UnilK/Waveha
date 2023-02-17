#pragma once

#include <valarray>
#include <vector>

namespace designa {

class Scolor {

public:

    Scolor(int framerate, float min_frequency, float decay_halftime);

    float process(float sample, float period);

    int get_shift_size();
    
    void set_shift(const std::vector<float> &new_shift);

    int get_delay();

private:

    float decay;
    int pointer, size, wsize, psize, state;
    std::valarray<float> ibuff, obuff, pbuff;
    std::vector<float> shift;

};

}

