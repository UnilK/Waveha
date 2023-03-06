#pragma once

#include "designf/delay.h"
#include "designf/enveloper.h"
#include "designf/pacer.h"
#include "designf/resampler.h"
#include "designf/splicer.h"

#include <deque>

namespace designf {

class Pitcher {

public:

    Pitcher(int frameRate, float minPitchHZ);

    float process(float sample);
    
    int get_delay();

    void set_colored_shift(float shift);
    void set_neutral_shift(float shift);

private:

    Pacer pacer;
    Enveloper enveloper;
    Delay delay;
    Splicer splicer;
    Resampler resampler;

    std::deque<float> ready;

};

}

