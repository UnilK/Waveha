#pragma once

#include <valarray>

namespace designd {

class Pacer {

public:

    Pacer(int rate, float low, float high, float calc_frequency = 200.0f);

    void push(float sample);
    float pull();

    int get_scale();
    float get_similarity();
    int get_delay();

private:

    void calc_scale();

    float strong_weight, weak_weight, decay, similarity;
    int size, min, max, mid, strong, weak, pop;
    int scale, calc_state, calc_period;
    std::valarray<float> buffer;

    const float epsilon = 0.001f;

};

}

