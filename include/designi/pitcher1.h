#pragma once

#include "designi/rbuffer.h"

#include <complex>
#include <vector>

namespace designi {

struct Wavelet1;

class Pitcher1 {

public:

    Pitcher1(int framerate, float minPitchHZ);

    float process(float sample);

    void set_absolute_pitch_shift(float shift);
    
    void set_color_shift(float shift);

    int get_delay();

    // structs to manage all the state variables

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

    struct PulseSegment {
       
        int left, right, length;
        float state;

        float slow_cut, fast_precut, fast_postcut;

        void push_left();
        void update_state(float shift);

        float calculate_position(float relative_shift);
    };

    struct Detector {
        
        float similarity;
        int min, max, pop, period;
        int calc_state, calc_period;

        bool tick();
    };

    struct Splitter {
        
        int state, length;
        std::vector<float> window, bit;
        std::vector<std::complex<float> > wavelet1, wavelet2;

        bool tick();
    };

    struct Debug {
        
        std::vector<float> mse;

    };
    Debug dbg;

private:

    void find_period();

    void apply_splitter();

    void move_segment();

    void apply_wavelet(Wavelet &w);

    HodgepodgeSet hpset;
    PulseSegment segment;
    Detector detector;
    Splitter splitter;

    rbuffer<float> ibuff, ebuff, lbuff, obuff;

};

}

