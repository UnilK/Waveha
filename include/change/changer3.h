#pragma once

#include <vector>

namespace change {

class Changer3 {

public:
    
    Changer3(int rate = 44100);

    float process(float sample);

    std::vector<float> debug();

};

}

