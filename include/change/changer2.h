#pragma once

#include <vector>
#include <complex>
#include <array>
#include <functional>

namespace change {

struct ChangerVars2 {
    int f_freqs;        // number of frequencies in echo transform
    float f_cutoff;     // lowpass frequency cutoff in (Hz)
    float f_decay;      // echo amplitude halving rate (seconds)
    float m_decay;      // momentum amplitude halving rate (seconds)
    float f_min;        // pitch min (Hz)
    float f_max;        // pitch max (Hz)
    int f_voice_reco;   // maximum amount of voice reconstruction frequencies
    int f_reco;         // maximum amount of reconstruction frequencies
    float c_reco_rate;  // reconstruction rate (Hz)
};

extern ChangerVars2 defaultVars2;

class Changer2 {

    // first function input is frequencies in Hz, second is magnitudes.
    typedef std::function<void(std::vector<float>&, std::vector<float>&)> func;

public:
    
    Changer2(float rate = 44100, ChangerVars2 var = defaultVars2);

    float process(float sample);

    void set_pitch_shift(float shift);
    void set_transform(func f);

    float get_pitch();

    std::vector<float> debug();

private:
    
    void update_reco();

    std::pair<float, float> para_peak(int i, float l, float m, float r);

    typedef std::complex<float> cfloat;

    int f_freqs, f_reco, f_voice_reco;
    float f_min, f_max, f_reso;
    float c_rate, f_cutoff, f_decay, m_decay;
    double c_power;

    std::vector<float> decay, momentum, dbg;
    std::vector<cfloat> root, echo;

    float f_shift, f_pitch_estimate;
    func f_transform;

    int r_period, r_slot, r_phase;
    std::vector<float> amplitude[4], window;
    std::vector<cfloat> phase[4], speed[4];

};

}

