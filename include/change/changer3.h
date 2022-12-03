#pragma once

#include <vector>
#include <complex>
#include <array>
#include <functional>
#include <deque>

namespace change {

struct ChangerVars3 {
    int f_freqs;        // number of frequencies in echo transform
    float f_min;        // lowest frequency in transform (Hz)
    float f_max;        // highest frequency in transform (Hz)
    float f_decay;      // echo transform amplitude halving rate (wavelengths)
    int e_freqs;        // envelope frequencies 
    float e_min;        // envelope min frequency
    float e_max;        // envelope max frequency
    float e_decay;      // envelope haling rate (seconds)
};

extern ChangerVars3 defaultVars3;

class Changer3 {

    typedef std::complex<float> cfloat;

public:
    
    Changer3(float rate = 44100);

    void set_framerate(float rate);

    void config(ChangerVars3 v);

    float process(float sample);

    void set_pitch_shift(float shift);

    std::vector<float> debug();

private:
   
    ChangerVars3 var;

    float c_rate;
    
    int f_freqs, e_freqs;
    float f_min, f_max, f_decay_factor;
    float e_min, e_max, e_decay;

    float f_shift;

    std::vector<float> f_decay;
    std::vector<cfloat> f_rota, e_rota;

    std::vector<cfloat> f_echo;
    std::vector<std::vector<cfloat> > f_envelope;
    std::vector<std::deque<float> > f_amp;

    std::vector<float> o_phase, o_rota;

};

}

