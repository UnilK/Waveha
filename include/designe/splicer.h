#pragma once

#include <valarray>
#include <vector>

namespace designe {

class Splicer {

public:

    Splicer(int frameRate, float minPitchHZ);

    std::vector<float> process(float sample, float energy, float period);

    void set_shift(float pitch_shift);

    int get_delay();

private:
    
    double shift, out;
    int iwsize, owsize, isize, osize, in, done, state;

    std::valarray<float> ibuff, ebuff, pbuff, obuff;

};

}

