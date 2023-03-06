#pragma once

#include <vector>

namespace designf {

class PeakFinder {

public:

    PeakFinder(int windowSize);

    bool process(float sample);

    int get_delay();

private:

    int index, size, tsize;
    std::vector<float> segtree;

};

}

