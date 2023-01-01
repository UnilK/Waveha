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

    float wl, wr, wo, decay;
    int min, max, size, left, right, old;
    std::vector<float> buffer, inv;

};

}

