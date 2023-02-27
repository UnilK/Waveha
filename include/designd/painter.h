#pragma once

#include <valarray>
#include <vector>

namespace designd {

class Painter {

public:

    Painter(int windowSize);

    float process(float sample);

    int get_shift_size();
    
    void set_shift(const std::vector<float> &new_shift);

    int get_delay();

private:

    int pointer, size, wsize, fsize, state;
    std::valarray<float> ibuff, obuff;
    std::vector<float> shift, window;

};

}

