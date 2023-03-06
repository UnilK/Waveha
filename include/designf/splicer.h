#pragma once

#include "designf/delay.h"
#include "designf/peakfinder.h"

#include <valarray>
#include <vector>

namespace designf {

class Splicer {

public:

    Splicer(int frameRate, float minPitchHZ, int minHopLength);

    std::vector<float> process(float sample, float energy);

    void set_shift(float pitch_shift);

    int get_delay();

private:
   
    PeakFinder peakfinder;
    Delay sdelay, edelay;

    float shift, out;
    int iwsize, owsize, isize, osize, in, done, state, hop;

    std::valarray<float> ibuff, ebuff, obuff;
    std::valarray<bool> pbuff; 

};

}

