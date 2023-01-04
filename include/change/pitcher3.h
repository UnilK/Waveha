#pragma once

#include <vector>

namespace change {

class Pitcher3 {

public:

    Pitcher3(double shift_, int rate_, float low);

    float process(float sample);

private:

    double shift;
    int rate, max, in, out, size, ssize, smax, wsize, state;
    std::vector<float> window, ibuff, obuff, previous;

};

}

