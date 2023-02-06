#pragma once

#include <vector>

namespace change {

class Phaser2 {

public:

    Phaser2(int rate, float low, float high,
            float vpop = 200.0f, float srek = 200.0f);

    void push(float sample);
    float pull();

    int get_scale();
    float get_similarity();
    int get_delay();

private:

    void calc_scale();
    int match(int source, int destination);

    float wl, wr, decay, similarity;
    int size, min, max, left, mid, right, pop;
    int old, scale, calc_state, calc_rate;
    std::vector<float> buffer;

};

}

