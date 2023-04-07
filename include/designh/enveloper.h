#pragma once

#include <designh/rbuffer.h>

#include <vector>
#include <complex>

namespace designh {

class Enveloper {

public:

    Enveloper(int windowSize);

    float process(float sample);

    int get_delay();

private:

    int wsize, state;
    rbuffer<float> ibuff, obuff;

    std::vector<float> window;

};

}

