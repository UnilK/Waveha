#pragma once
#include "change/util.h"

#include <vector>
#include <complex>
#include <array>
#include <functional>
#include <deque>

namespace change {

struct ChangerVars2 {
    int f_freqs;        // number of frequencies in echo transform
    float f_cutoff;     // lowpass frequency cutoff in (Hz)
    float f_decay;      // echo amplitude halving rate (seconds)
    float f_window;     // echo transfrom window length (seconds)
    float m_decay;      // momentum amplitude halving rate (seconds)
    float f_min;        // pitch min (Hz)
    float f_max;        // pitch max (Hz)
    int f_reco;         // maximum amount of voice reconstruction frequencies
    float r_rate;       // reconstruction rate (Hz)
    float r_window;     // reconstruction input window length (seconds)
    int v_track;        // number of harmonics that are tracked.
};

extern ChangerVars2 defaultVars2;

class Changer2 {

public:
    
    Changer2(float rate = 44100, ChangerVars2 var = defaultVars2);

    float process(float sample);

    void set_pitch_shift(float shift);

    float get_pitch();

    std::vector<float> debug();

private:
    
    void update_reco();

    std::pair<float, float> para_peak(int i, float l, float m, float r);
    void smooth_clip(std::vector<float> &data);

    typedef std::complex<float> cfloat;

    int c_size;
    rvec in, out;

    int f_freqs, f_reco, r_phase, r_period;
    float f_min, f_max, f_reso, f_decay, f_cutoff;
    float c_rate, m_decay;

    std::vector<float> momentum, dbg;
    std::vector<cfloat> root, echo;

    float f_shift, f_pitch;

    int r_size, r_delay, p_size;
    std::vector<float> r_window, p_momentum;

    int v_size;
    float v_pitch;
    std::vector<double> v_phase;

};

}

