#pragma once

#include <vector>

namespace change {

class Phaser3 {

public:

    Phaser3(int rate, float low, float high);

    void push(float sample);
    float pull();

private:

    int period();

    int min, max, size, now, old, state, twindow;
    std::vector<float> buffer;

};

}

