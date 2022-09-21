#pragma once

#include <vector>
#include <complex>
#include <deque>
#include <array>

namespace change {

struct ChangerVars {
    float c_quiet_limit;
    float p_rate;       // pitch update rate, Hz
    float p_cutoff;     // minimum limit for forced pitch catch
    float p_decay;      // momentum decay
    float p_confident;  // confidence limit for pitch quesses.
    float p_pid_p;      // pitch output pid controller
    float p_pid_i;
    float p_pid_d;
    float p_follow;
    float p_remember;
    float f_cutoff;     // voice extraction lowpass limit
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
            float low = 60,                     // pitch search range lower bound, Hz.
            float high = 900,                   // pitch search range upper bound, Hz.
            ChangerVars setup = defaultVars);   // variables

    // c_size = 2 * ceil(framerate / low)

    // feed samples to be processed. Works in O(c_size) and has latency of c_size.
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
    static const int v_max = 64;

    typedef std::complex<float> cfloat;

    // short time fourier transform for voice de/reconstruction.
    void update_short();
    void recalc_short(int index);

    // mse for pitch detection
    void update_mse();

    // refining methods for pitch tracking
    void update_pitch();
    void refine_pitch(int block);
    void move_pitch();
    
    int left(const int&) const;
    int mid(const int&) const;
    int right(const int&) const;

    // framerate
    float c_rate;

    // rolling caches
    rvec c_raw, c_out;

    // short time ft and the used periods.
    std::vector<double> f_short_r, f_short_i;
    std::vector<int> f_short_period, f_short_index;

    // energy envelopes of the short time fourier transforms
    std::vector<rvec> f_envelope;

    // pitch tracking
    std::vector<double> p_mse1, p_mse2;
    std::vector<float> p_nmse, p_momentum, p_norm;

    // tickers for load balancing
    ticker f_short_t, p_norm_t, c_short_t;

    // pitch search range
    int p_min, p_max, c_size;
    float p_high, p_low;

    int m_block, t_block, p_block;          // period integer part
    float m_period, t_period, p_period;     // accurate period
    float m_pitch, t_pitch, p_pitch;        // accurate pitch
    float p_confidence;                     // confidence in pitch assesment.
    float p_pitch_avg;

    PID p_pid;                  // pitch PID controller

    // precalculated tables
    std::vector<float> x_inv;
    std::vector<double> x_root_r, x_root_i;

};

}

