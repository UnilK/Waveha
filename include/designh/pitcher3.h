#pragma once

#include "designh/rbuffer.h"
#include "designh/enveloper.h"
#include "designh/rsegtree.h"

#include <complex>
#include <vector>

namespace designh {

class Pitcher3 {

public:

    Pitcher3(int framerate, float minPitchHZ);

    float process(float sample);

    void set_absolute_pitch_shift(float shift);

    void set_color_shift(float shift);

    int get_delay();

    struct Wavelet {

        void increment();
        
        float frequency_response(float framerate, float frequency);

        float length, state, spins, gain, shift;
    };
    
    struct HodgepodgeSet {
        
        void init(std::string file);

        std::vector<Wavelet> wavelets;

        float max_length, shift;

        const float max_shift = 8.0f;
    };
    
    struct Detector {
        
        float similarity;
        int min, max, pop, period;
        int calc_state, calc_period;

        bool tick();
    };

private:

    float pointer;

    void find_period();

    void apply_wavelet(Wavelet &w);

    HodgepodgeSet hpset;
    Detector detector;
    
    rbuffer<float> ibuff, obuff;

};


}

