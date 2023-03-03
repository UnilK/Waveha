#pragma once

#include "designe/pacer.h"
#include "designe/resampler.h"
#include "designe/splicer.h"
#include "designe/splitter.h"

#include <deque>

namespace designe {

class Pitcher {

public:

    Pitcher(int frameRate, float minPitchHZ);

    float process(float sample);
    
    int get_delay();

    void set_colored_shift(float shift);
    void set_neutral_shift(float shift);

private:

    void update_shifts();

    Splitter splitter;
    Pacer pacer;
    Splicer splicer;
    Resampler resamplerLow, resamplerHigh;

    std::deque<float> lowPipe, lowReady, highReady;

    float colorShift, neutralShift, totalShift;

};

}

