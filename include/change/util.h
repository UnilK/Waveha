#pragma once

#include <random>

namespace change {

extern std::mt19937 rng32;
double rnd();
double rnd(double);
double rnd(double, double);

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

    float &operator[](int i){ return data[index + i]; }
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

}

