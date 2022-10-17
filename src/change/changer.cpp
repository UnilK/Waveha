#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#include "change/changer.h"
#include "math/ft.h"
#include "math/constants.h"

#include <cassert>
#include <algorithm>
#include <cmath>
#include <string.h>

#include <iostream>
#include <iomanip>

namespace change {

PID::PID(float p_, float i_, float d_, float dt_) : p(p_), i(i_), d(d_) {
    dt = dt_;
    y = 0.0f;
    ie = e = de = pe = 0.0f;
}

void PID::initialize(float y_){
    y = y_;
    ie = e = de = pe = 0.0f;
}

float PID::get(float target){

    pe = e;
    e = target - y;
    ie += e * dt;
    de = (e - pe) / dt;

    y += p * e + i * ie + d * de;
    
    return y;
}

void rvec::init(int size_, int index_, float val){
    size = size_;
    buffer = 8;
    length = 2 * buffer + 8 * size;
    index = std::min(length - buffer - size - 1, std::max(buffer, index_ + buffer));
    if(data != nullptr) delete[] data;
    data = new(std::align_val_t(32)) float[length];
    for(int i=0; i<length; i++) data[i] = val;
}

rvec::~rvec(){
    delete[] data;
}

float &rvec::operator[](int i){ return data[index + i]; }

void rvec::push(const float &x){
    
    index++;
    if(index + size + buffer >= length){
        assert(index + size + buffer == length && index >= buffer);
        memmove(data, data + index - buffer, (size + buffer) * sizeof(float));
        index = buffer;
    }

    data[index + size - 1] = x;
}

ticker::ticker(int period_, int speed_, int index_) :
    period(period_),
    speed(speed_),
    index(index_),
    sindex(0)
{}

int ticker::tick(){
    sindex = (sindex + 1) % speed;
    if(sindex == 0){
        index = (index + 1) % period;
        return index;
    }
    return -1;
}

ChangerVars defaultVars {
    .c_quiet_limit = 1e-5f,
    .p_rate = 5000,
    .p_cutoff = 0.15,
    .p_decay = 0.001f,
    .c_decay = 0.0005f,
    .r_decay = 0.002f,
    .rp_decay = 0.05f,
    .p_voiced = 0.6f,
    .p_pid_p = 0.05f,
    .p_pid_i = 1.0f,
    .p_pid_d = 1e-5f,
    .r_cutoff = 6000.0f,
    .c_cutoff = 12000.0f,
    .r_freqs = 64,
};

Changer::Changer(float rate, float low, float high, ChangerVars setup) :
    var(setup)
{
    {   // parse input parameters

        assert(rate >= 2.0f && low > 0.0f && high > 0.0f);
        assert(low <= high);
        assert(rate / low < 1e9);
        high = std::min(high, rate / 2);
        var.r_cutoff = std::min(var.r_cutoff, rate / 2);
        var.c_cutoff = std::min(var.c_cutoff, rate / 2);
    }

    {   // apply basic input parameters
        
        c_rate = rate;
        p_low = low;
        p_high = high;
    }
    
    {   // calculate integer period lengths for pitch search range.
        
        p_min = std::ceil(c_rate / p_high);
        p_max = std::ceil(c_rate / p_low);
        c_size = 2 * p_max;
    }

    {   // initialize rolling caches. Balance memmove load.

        int pos = 8 * c_size / 3;

        c_raw.init(c_size, 0 * pos);
        c_out.init(c_size, 1 * pos);
        r_phase.init(c_size, 2 * pos);
    }

    {   // initialize pitch tracking
        
        p_norm_t = ticker(std::ceil(c_rate / var.p_rate), 1, 0);

        p_mse1.resize(p_max + 1, 0.0);
        p_mse2.resize(p_max + 1, 0.0);
        p_nmse.resize(p_max + 1, 0.0f);
        p_momentum.resize(p_max + 1, 0.0f);

        p_decay = std::pow(0.5f, 1.0f / (var.p_decay * c_rate));

        p_pitch = mp_pitch = t_pitch = 120;
        p_period = t_period = c_rate / p_pitch;
        p_block = t_block = std::round(t_period);

        p_pid = PID(var.p_pid_p, var.p_pid_i, var.p_pid_d, 1.0f / c_rate);
        p_pid.initialize(t_pitch);
        
        m_pid = PID(var.p_pid_p, var.p_pid_i, var.p_pid_d, 1.0f / c_rate);
        m_pid.initialize(t_pitch);

        p_confidence = 0.0;
        n_confidence = 1.0f;
        v_confidence = 0.0f;
    }

    {   // initilize deconstruction / reconstruction
        
        r_decay = std::pow(0.5f, 1.0f / (var.r_decay * c_rate));
        rp_decay = std::pow(0.5f, 1.0f / (var.rp_decay * c_rate));
        c_decay = std::pow(0.5f, 1.0f / (var.c_decay * c_rate));
        
        c_freqs = std::floor(var.c_cutoff / (c_rate / p_max));
        r_freqs = var.r_freqs;

        c_freq.resize(c_freqs, 0.0);
        r_freq.resize(r_freqs, 0.0);
        t_freq.resize(r_freqs, 0.0);
        r_psum.resize(r_freqs, 0.0f);
        c_amplitude.resize(c_freqs, 0.0);
        r_amplitude.resize(var.r_freqs, 0.0);
        
        c_speed.resize(c_freqs, 0.0);
        for(int i=0; i<c_freqs; i++) c_speed[i] = 2 * PI * (i + 1) / p_max;

        r_left = 0;
    }

    {   // precalculate tables

        x_inv.resize(c_size + 1, 0.0f);
        for(int i=1; i<=c_size; i++) x_inv[i] = 1.0 / i;
    }
}

float Changer::process(float sample){

    sample = cut16(sample);

    c_raw.push(sample);
    c_out.push(0.0f);

    update_mse();
   
    if(p_norm_t.tick() == 0) update_pitch();

    move_pitch();

    update_noise();
    update_voice();

    reconstruct();

    return c_out[right(0)];
}

int Changer::get_latency() const { return c_size; }
float Changer::get_pitch() const { return p_pitch; }

float Changer::get_confidence() const {
    return p_confidence;
}

bool Changer::is_voiced() const {
    return v_confidence > n_confidence;
}

std::vector<float> Changer::debug_mse(){
    std::vector<float> x(r_freqs);
    for(int i=0; i<r_freqs; i++) x[i] = std::arg(r_psum[i]);
    return x;
}

void Changer::update_mse(){

    // update mse by moving the windows.

    for(int i=1; i<=p_max; i++){

        const float &fl1 = c_raw[left(0)];
        const float &fr1 = c_raw[left(i)];
        const float &sl1 = c_raw[right(-i)];
        const float &sr1 = c_raw[right(0)];

        const float &fl2 = c_raw[right(-2*i)];
        const float &fr2 = c_raw[right(-i)];
        const float &sl2 = c_raw[right(-i)];
        const float &sr2 = c_raw[right(0)];

        p_mse1[i] += (double)((sr1-sl1)*(sr1-sl1)) - (double)((fr1-fl1)*(fr1-fl1));
        p_mse2[i] += (double)((sr2-sl2)*(sr2-sl2)) - (double)((fr2-fl2)*(fr2-fl2));
    }
}

void Changer::update_pitch(){

    {   // the sacred nonsense. Change at your own risk.
        
        double sum = 0.0f;
        for(int i=1; i<=p_max; i++){
            float e1 = (float)(p_mse1[i]) * x_inv[c_size-i];
            float e2 = (float)p_mse2[i] * x_inv[i];
            p_nmse[i] = e1 * n_confidence + e2 * v_confidence + 1e-7f;
            sum += p_nmse[i];
            p_nmse[i] *= i / sum;
        }

        auto tmp = p_nmse;

        const int N = 8;
        double z = (2*N + 1);
        double iz = 1 / z;
        sum = z * p_nmse[1];

        for(int i = 1-N; i<=p_max; i++){
            if(i+N <= p_max) sum += tmp[i+N];
            else sum += tmp[p_max];
            if(i-N-1 > 0) sum -= tmp[i-N-1];
            else sum -= tmp[1];
            if(i > 0) p_nmse[i] = sum * iz;
        }
    }

    for(int i=0; i<=p_max; i++){
        p_momentum[i] = p_momentum[i] * p_decay + p_nmse[i] * (1.0f - p_decay);
    }

    {   // pick a peak. todo: improve.

        float best = 1e9;
        int n_block = p_max;

        int left = std::max(p_min, 2);
        int right = p_max - 2;
        for(int i=left; i<=right && best > var.p_cutoff; i++){
            if(     p_momentum[i-1] > p_momentum[i] &&
                    best > p_momentum[i] &&
                    p_momentum[i+1] > p_momentum[i] &&
                    p_momentum[i+2] > p_momentum[i] &&
                    p_momentum[i-2] > p_momentum[i]){
                best = p_momentum[i];
                n_block = i;
            }
        }

        if(best < var.p_voiced){
            p_confidence = std::max((var.p_voiced - best) / var.p_voiced, 0.0f);
            v_confidence = 0.5f + p_confidence * 0.5f;
            n_confidence = 1.0f - v_confidence;
        } else {
            p_confidence = std::min(1.0f, (best - var.p_voiced) / (1.0f - var.p_voiced));
            n_confidence = 0.5f + p_confidence * 0.5f;
            v_confidence = 1.0f - n_confidence;
        }

        if(is_voiced()) refine_pitch(n_block);
    }
}

void Changer::refine_pitch(int block){

    p_pitch = c_rate / block;
    
    if(block > 1 && block + 1 <= p_max){
        
        float l = p_momentum[block-1];    // (-1, l)
        float m = p_momentum[block];      // (0, m)
        float r = p_momentum[block+1];    // (1, r)

        // construct a parabola using the three points around
        // the peak. set the peak as origo and find the
        // bottom of the parabola.

        // y = ax^2 + bx + c

        float y0 = l - m, y1 = r - m;
        float a = (y1 + y0) / 2;
        float b = (y1 - y0) / 2;

        float bottom = -b / (2*a);

        if(bottom > -1.0f && bottom < 1.0f){
            p_pitch = c_rate / (block + bottom);
        }
    }

    mp_pitch = mp_pitch * p_decay + p_pitch * (1.0f - p_decay);
    p_period = c_rate / p_pitch;
    p_block = std::round(p_period);
}

void Changer::move_pitch(){
    
    t_pitch = p_pid.get(p_pitch);
    t_period = c_rate / t_pitch;
    t_block = std::round(t_period);
}

void Changer::update_noise(){

}

void Changer::update_voice(){

    {
        float p = r_phase[right(0)] + mp_pitch / c_rate;
        if(is_voiced()) p = r_phase[right(0)] + p_pitch / c_rate;
        if(p > 1) p -= 2.0f;
        r_phase.push(p);
    }

    r_left--;

    for(int i=0; i<r_freqs; i++){
        r_freq[i] += c_raw[right(0)] * math::dexp(-r_phase[right(0)]*(i+1));
    }

    while(1){
        float r = r_phase[right(0)];
        float l = r_phase[right(r_left)];
        float d = r - l;
        if(d < 0.0f) d += 2;
        if(d <= 1.0f) break;
        for(int i=0; i<r_freqs; i++){
            r_freq[i] -= c_raw[right(r_left)] * math::dexp(-r_phase[right(r_left)]*(i+1));
        }
        r_left++;
    }

    for(int i=0; i<r_freqs; i++){
        r_amplitude[i] = r_amplitude[i] * r_decay + std::abs(r_freq[i]) * (1.0f - r_decay);
        r_psum[i] = r_psum[i] * r_decay + (1.0f - r_decay) * (std::complex<float>)r_freq[i];
    }
}

void Changer::reconstruct(){

    float &out = c_out[right(0)];

    float ilen = 1.0f / (1-r_left);
    for(int i=0; i<r_freqs; i++){
        float d = std::abs(r_psum[i]);
        float dd = r_amplitude[i] * ilen;
        if(d < 1e-9f) continue;
        out += (r_psum[i] / d * dd * math::dexp(r_phase[right(0)]*(i+1))).real();
    }

}

int Changer::left(const int &i) const { return i; }
int Changer::mid(const int &i) const { return i + p_max; }
int Changer::right(const int &i) const { return i + c_size - 1; }

float Changer::cut16(float sample){
    const float b15 = 1<<15;
    const float i15 = 1.0f / (1<<15);
    std::modf(sample * b15, &sample);
    sample *= i15;
    return sample;
}

}

