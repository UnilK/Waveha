#pragma once

#include <vector>
#include <complex>
#include <deque>
#include <array>

namespace change {

struct ChangerVars {
    float p_low;
    float p_high;
    float c_quiet_limit;
    float p_rate;       // pitch update rate, Hz
    float p_cutoff;     // minimum limit for forced pitch catch
    float p_decay;      // pitch detection momentum decay
    float pm_decay;     // pitch average decay rate.
    float c_decay;      // noise amplitude distribution decay.
    float r_decay;      // voice amplitude distribution decay.
    float rp_decay;     // voice phase distribution decay.
    float p_voiced;     // confidence limit for pitch quesses.
    float p_pid_p;      // pitch output pid controller
    float p_pid_i;
    float p_pid_d;
    float r_cutoff;     // voiced part extraction lowpass limit.
    float c_cutoff;     // noise extraction lowpass limit.
    int r_freqs;        // max number of frequencies for voiced part tracking
};

extern ChangerVars defaultVars;

struct PID {

    // PID controller for controlling the output pitch.

    PID(float p_ = 1.0f, float i_ = 1.0f, float d_ = 1.0f, float dt_ = 1.0f);

    void initialize(float _y);

    float get(float target);

    float p, i, d;
    float y, dt, ie, e, de, pe;

};

struct rvec {
    
    // rolling vector - a specialezed deque-like structure
    // simiplified to the bare essentials to make it easy for the
    // compiler to use SIMD instructions and to reduce cache load.

    void init(int size_ = 1, int index_ = 1, float val = 0.0f);
    ~rvec();

    float &operator[](int i);
    void push(const float &x);

    int size, buffer, index, length;
    float *data = nullptr;

};

struct ticker {

    // ticker class for load balancing.

    ticker(int period_ = 1, int speed_ = 1, int index_ = 0);

    int tick();

    int period, speed, index, sindex;

};

class Changer {

public:
    
    Changer(
            float rate = 44100,                 // audio rate sample rate.
            ChangerVars setup = defaultVars);   // variables

    // c_size = 2 * ceil(framerate / low)

    // feed samples to be processed. Works in O(c_size).
    // Latency of c_size samples.
    float process(float sample);
    
    // get the latency. Returns c_size
    int get_latency() const;

    // return pitch assesment
    float get_pitch() const;

    // returns pitch assesment confidence
    float get_confidence() const;
    bool is_voiced() const;

    // for development & debugging
    std::vector<float> debug_mse();

private:
    
    ChangerVars var;

    typedef std::complex<float> cfloat;
    typedef std::complex<double> cdouble;

    // mse for pitch detection
    void update_mse();

    // update noise frequency distribution
    void update_noise();
    
    // update voice frequency distribution
    void update_voice();

    // reconstruct voice and noise from distributions.
    void reconstruct();

    // refining methods for pitch tracking
    void update_pitch();
    void refine_pitch(int block);
    void move_pitch();
    
    int left(const int&) const;
    int mid(const int&) const;
    int right(const int&) const;

    // cut float to 15 bits of mantissa, no exponent, sign is preserved.
    float cut16(float);

    // framerate
    float c_rate;

    // rolling caches
    rvec c_raw, c_out, r_phase;

    // pitch tracking
    std::vector<double> p_mse1, p_mse2;
    std::vector<float> p_nmse, p_momentum;

    // tickers for load balancing
    ticker p_norm_t;

    // pitch search range
    int p_min, p_max, c_size;
    float p_high, p_low;

    int t_block, p_block;                   // period integer part (samples)
    float t_period, p_period, pm_period;    // accurate period (samples)
    float t_pitch, p_pitch, pm_pitch;       // accurate pitch (Hz)
    float p_confidence;                     // confidence in classification. (0.0 - 1.0)
    float v_confidence;                     // voiced confidence
    float n_confidence;                     // noise confidence
    float r_decay, rp_decay, c_decay;       // decay rates
    float p_decay, pm_decay;

    // pitch PID controller
    PID p_pid, m_pid;

    // deconstruction and reconstruction
    std::vector<cdouble> c_freq, r_freq, t_freq;
    std::vector<cfloat> r_psum;
    std::vector<double> c_amplitude, r_amplitude;
    std::vector<float> c_phase, c_speed;
    int c_freqs, r_freqs, r_left;

    // precalculated tables
    std::vector<float> x_inv;

    int xz;
    std::vector<float> x_dec, x_mom;
    std::vector<cfloat> x_et, s_et, y_et;

};

}

