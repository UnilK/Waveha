#pragma once

#include <vector>
#include <complex>
#include <array>

namespace change {

struct DetectorVars2 {
    int f_freqs;        // number of frequencies in echo transform
    float f_cutoff;     // lowpass frequency cutoff in (Hz)
    float f_decay;      // echo amplitude halving rate (seconds)
    float f_window;     // echo transfrom window length (seconds)
    float m_decay;      // momentum amplitude halving rate (seconds)
    int f_reco;         // maximum number of recognized frequency peaks.
};

extern DetectorVars2 ddefaultVars2;

class Detector2 {

public:
    
    Detector2(int rate, float low, float high,
            DetectorVars2 var = ddefaultVars2);

    void push(float sample);
    float get_pitch();

    std::vector<float> debug();

private:
    
    std::pair<float, float> para_peak(int i, float l, float m, float r);

    typedef std::complex<float> cfloat;

    int c_size;
    int f_freqs, f_reco;
    float f_min, f_max, f_reso, f_decay, f_cutoff;
    float c_rate, m_decay, f_pitch;

    std::vector<float> momentum;
    std::vector<cfloat> root, echo;
};

}

