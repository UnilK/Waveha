#pragma once

#include <vector>

namespace change {

class Phaser4 {

public:

    Phaser4(int rate, float low, float high);

    void push(float sample);
    float pull();

private:

    int period(int);

    float wl, wr, wo, decay;
    int min, max, size, left, right, old;
    std::vector<float> buffer;

};

}

