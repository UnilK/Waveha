#pragma once

#include <valarray>

namespace designf {

class Delay {

public:

    Delay();
    Delay(int delayInSamples);

    void resize(int delayInSamples);

    float process(float sample);

    int get_delay();

private:
    
    int pointer, delay, size;
    std::valarray<float> buffer;

};

}

