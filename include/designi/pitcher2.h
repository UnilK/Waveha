#pragma once

#include "designi/rbuffer.h"

#include <complex>
#include <vector>
#include <deque>

namespace designi {

class Pitcher2 {

public:

    Pitcher2(int framerate, float minPitchHZ);

    float process(float sample);

    void set_absolute_pitch_shift(float shift);
    
    void set_color_shift(float shift);

    int get_delay();

    // structs to manage all the state variables

    struct Wavelet {

        float frequency_response(float framerate, float frequency);

        float target_shift, shift;
        float length, state, spins, gain, frequency;
    };

    struct HodgepodgeSet {
        
        void init(std::string file, float frame_rate);

        std::vector<Wavelet> wavelets;

        float max_length;

        const float max_shift = 8.0f;
    };

    struct PulseSegment {
       
        int left, right;
        float pointer, shift;
        
        float previous_peak;

        float zero_offset;
        float slow_cut, fast_precut, fast_postcut;

        int offset;
        std::deque<int> peaks;

    };

    struct Detector {
        
        float similarity, pitch;
        int min, max, pop, period;
        int calc_state, calc_period;

        bool tick();
    };

    struct Splitter {
        
        int state, length, tick_period;
        std::vector<float> window, bit;
        std::vector<std::complex<float> > wavelet1, wavelet2;
        float energy_weight, lowpass_weight;

        bool tick();
    };

    struct Debug {
        
        std::vector<float> mse;

    };

    Debug dbg;

private:

    void find_period();

    void apply_splitter();

    bool add_peak();
    
    void push_segment();

    void update_pointer();
    
    void update_segment();

    void update_shifts();

    std::vector<float> calculate_positions(float shift);
    
    void apply_wavelet(Wavelet &w, float input_position);

    float frame_rate;

    HodgepodgeSet hpset;
    PulseSegment segment;
    Detector detector;
    Splitter splitter;

    rbuffer<float> ibuff, ebuff, obuff;

};

}

