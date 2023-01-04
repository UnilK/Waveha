#pragma once

#include <vector>

namespace change {

class Phaser {

public:

    Phaser(int rate, float low, float high);

    void push(float sample);
    float pull();

private:

    int period();

    int min, max, size, left, dist, right, out, state;
    std::vector<float> ibuff, obuff, inv, window;

};

}

