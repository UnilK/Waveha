#pragma once

#include <vector>

namespace change {

class Phaser2 {

public:

    Phaser2(int rate, float low, float high);

    void push(float sample);
    float pull();

    int period();
    float pitch();

    std::vector<float> debug();

private:

    int rate, min, max, size, in, out, pass, wsize, fsize, msize, wstate, fstate;
    std::vector<float> ibuff, obuff, lowpass, window, filter, inv;

};

}

