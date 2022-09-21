// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")
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
    .p_decay = 0.6f,
    .p_confident = 0.3f,
    .p_pid_p = 0.05f,
    .p_pid_i = 1.0f,
    .p_pid_d = 1e-5f,
    .p_follow = 0.1f,
    .p_remember = 0.01f,
    .f_cutoff = 6000
};

Changer::Changer(float rate, float low, float high, ChangerVars setup) :
    var(setup)
{
    {   // parse input parameters

        assert(rate >= 2.0f && low > 0.0f && high > 0.0f);
        assert(low <= high);
        assert(rate / low < 1e9);
        high = std::min(high, rate / 2);
        var.f_cutoff = std::min(var.f_cutoff, rate / 2);
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

        int pos = 8 * c_size / 2;

        c_raw.init(c_size, 0 * pos);
        c_out.init(c_size, 1 * pos);
    }

    {   // initialize online fourier transforms
        
        for(int i=std::max(1, (int)std::floor(c_rate / var.f_cutoff)); i<=p_max; ){
            f_short_period.push_back(i);
            float j = i; i++;
            while(c_rate/j - c_rate/i < 1.0f) i++;
        }

        assert(!f_short_period.empty());

        if(f_short_period.back() != p_max) f_short_period.push_back(p_max);
        f_short_r.resize(f_short_period.size(), 0.0f);
        f_short_i.resize(f_short_period.size(), 0.0f);

        f_short_index.resize(p_max+1, 0);
        for(int i=1, j=0; i<=p_max; i++){
            if(i > f_short_period[j]) j++;
            f_short_index[i] = j;
        }

        f_envelope.resize(f_short_period.size());

        int pos = 8 * p_max / f_short_period.size();
        for(unsigned i=0; i<f_envelope.size(); i++){
            f_envelope[i].init(p_max, pos * i);
        }
    }
    
    {   // initialize tickers

        f_short_t = ticker(f_short_period.size(), 1, 0);
        p_norm_t = ticker(std::ceil(c_rate / var.p_rate), 1, 0);
    }
   
    {   // initialize pitch tracking

        p_mse1.resize(p_max + 1, 0.0);
        p_mse2.resize(p_max + 1, 0.0);
        p_nmse.resize(p_max + 1, 0.0f);
        p_momentum.resize(p_max + 1, 0.0f);
        p_norm.resize(p_max + 1, 0.0f);

        var.p_decay = std::pow(var.p_decay, 1.0f / (c_rate / (50.0f * p_norm_t.period)));

        p_pitch_avg = m_pitch = t_pitch = p_pitch = 120;
        m_period = t_period = p_period = c_rate / p_pitch;
        m_block = t_block = p_block = std::round(p_period);

        p_pid = PID(var.p_pid_p, var.p_pid_i, var.p_pid_d, 1.0f / c_rate);
        p_pid.initialize(t_pitch);
    }

    {   // precalculation tables

        x_inv.resize(c_size + 1, 0.0f);
        for(int i=1; i<=c_size; i++) x_inv[i] = 1.0 / i;
        
        for(int i : f_short_period){
            auto x = std::polar(1.0, 2.0 * PI / i);
            x_root_r.push_back(x.real());
            x_root_i.push_back(x.imag());
        }
    }
}

float Changer::process(float sample){

    std::modf(sample*(1<<16), &sample);
    sample /= (1<<16);

    c_raw.push(sample);
    c_out.push(0.0f);

    update_short();
    update_mse();
   
    if(p_norm_t.tick() == 0) update_pitch();

    move_pitch();

    return 0.0f;
}

int Changer::get_latency() const { return c_size; }
float Changer::get_pitch() const { return m_pitch; }

float Changer::get_confidence() const {
    return p_confidence;
}

bool Changer::is_voiced() const {
    return p_confidence > var.p_confident;
}

unsigned lol = 0;
std::vector<float> Changer::debug_mse(){
    std::vector<float> v(p_max, 0.0f);
    lol = (lol + 1) % f_envelope.size();
    for(int i=0; i<p_max; i++) v[i] = f_envelope[lol][i];
    return v;
}

void Changer::update_short(){

    // weird online short-time fourier transform

    for(unsigned x=0; x<f_short_period.size(); x++){
        
        const int &p = f_short_period[x];
        
        // do complex multiplication by hand to make it easier to vectorize.
        // (vectorize by hand later on if needed).

        // implementation with std::complex
        // f_short[x] = (f_short[x] + c_raw[right(0)] - c_raw[right(-p)]) * x_root[x];
        
        float rr = (f_short_r[x] + c_raw[right(0)] - c_raw[right(-p)]) * x_root_r[x]
            - f_short_i[x] * x_root_i[x];
        
        float ii = (f_short_r[x] + c_raw[right(0)] - c_raw[right(-p)]) * x_root_i[x]
            + f_short_i[x] * x_root_r[x];

        f_short_r[x] = rr;
        f_short_i[x] = ii;

        f_envelope[x].push((rr*rr + ii*ii) * x_inv[p] * x_inv[p]);
    }
    
    recalc_short(f_short_t.tick());
}

void Changer::recalc_short(int index){

    if(index < 0) return;

    int x = index;
    f_short_r[x] = f_short_i[x] = 0.0f;

    for(int i=1-x; i<=0; i++){
        
        float rr = (f_short_r[x] + c_raw[right(i)]) * x_root_r[x]
            - f_short_i[x] * x_root_i[x];
        
        float ii = (f_short_r[x] + c_raw[right(i)]) * x_root_i[x]
            + f_short_i[x] * x_root_r[x];

        f_short_r[x] = rr;
        f_short_i[x] = ii;
    }
}

void Changer::update_mse(){

    // update mse by moving the windows.

    for(int i=1; i<=p_max; i++){

        const float &fl = c_raw[left(-1)];
        const float &fr = c_raw[left(i-1)];
        const float &sl = c_raw[right(-i)];
        const float &sr = c_raw[right(0)];

        const float &l = c_raw[mid(-i-1)];
        const float &m = c_raw[mid(-1)];
        const float &r = c_raw[mid(i-1)];

        p_mse1[i] += (double)((sr-sl)*(sr-sl)) - (double)((fr-fl)*(fr-fl));
        p_mse2[i] += (double)((r-m)*(r-m)) - (double)((m-l)*(m-l));
    }
}

void Changer::update_pitch(){

    // the sacred nonsense. Change at your own risk.

    {
        float sum = 0.0f;

        float w1 = std::min(0.7f, std::max(0.05f, 1.0f - p_confidence));
        float w2 = 1.0f - w1;
        float c = var.c_quiet_limit / (2 * p_max);

        for(int i=1; i<=p_max; i++){
            p_nmse[i] = (2 * p_max - i) * c + (float)p_mse1[i] * x_inv[c_size-i] * w1
                + w2 * (float)p_mse2[i] * x_inv[i];
            sum += p_nmse[i];
            p_nmse[i] *= i / sum;
        }
    }

    for(int i=0; i<=p_max; i++){
        p_momentum[i] = p_momentum[i] * var.p_decay + p_nmse[i];
    }

    float avg = 0.0f;
    for(int i=p_min; i<=p_max; i++) avg += p_momentum[i];
    avg /= (p_max-p_min+1);

    if(avg > 1e-18){
        float iavg = 1.0f / avg;
        for(int i=0; i<=p_max; i++) p_norm[i] = p_momentum[i] * iavg;
    }

    // find minimum peaks

    const int N = 32;
    const int M = std::min(N/2 - 1, std::max(p_min/2, 1));
    const int L = 2*M+1;

    std::vector<float> segtree(2*N, 1e9);
    auto add = [&](int i, const float &o) -> void {
        i = (i%L)+N;
        segtree[i] = o;
        for(i /= 2; i; i /= 2) segtree[i] = std::min(segtree[2*i], segtree[2*i+1]);
    };

    for(int i=1; i<=std::min(p_min+M-1, p_max); i++) add(i, p_norm[i]);

    float best = 1e9;
    int n_block = p_max;
    avg = 0.0f;

    for(int i=p_min; i<=p_max && best > var.p_cutoff; i++){

        if(i+M <= p_max) add(i+4, p_norm[i+M]);
        else add(i+M, 1e9);

        avg += p_norm[i];
        if(segtree[1] < best){
            n_block = i;
            best = p_norm[i];
        }
    }

    avg /= (p_max-p_min+1);

    p_confidence = 1.0f - (p_norm[n_block] / avg);

    if(p_confidence > var.p_confident) refine_pitch(n_block);
}

void Changer::refine_pitch(int block){

    p_period = block;
    p_pitch = c_rate / block;
    
    if(block > 1 && block + 1 <= p_max){
        
        float l = p_norm[t_block-1];
        float m = p_norm[t_block];
        float r = p_norm[t_block+1];

        // construct a parabola using the three points around
        // the peak. set the peak as origo and find the
        // bottom of the parabola. val[0] and val[2] are at distance
        // 1 from val[1] on the x-axis.

        // y = ax^2 + bx

        float y0 = l - m, y1 = r - m;
        float a = (y1 + y0) / 2;
        float b = (y1 - y0) / 2;

        float bottom = -b / (2*a);

        if(bottom > -1.0f && bottom < 1.0f){
            p_period = p_block + bottom;
            p_pitch = c_rate / p_period;
        }
    }

    p_block = std::round(p_period);

    float c = p_confidence * var.p_follow *
        std::exp(1.0f * (1 - std::max(m_pitch, p_pitch)/std::min(m_pitch, p_pitch)));
    
    if(std::max(p_pitch, p_pitch_avg)/std::min(p_pitch, p_pitch_avg) > 3.0f) c *= 1e-5f;
    
    m_pitch = c * p_pitch + (1.0f - c) * m_pitch;
    m_period = c_rate / m_pitch;
    m_block = std::round(m_period);

    p_pitch_avg = var.p_remember * m_pitch + (1.0f - var.p_remember) * p_pitch_avg;
}

void Changer::move_pitch(){
    
    t_pitch = p_pid.get(m_pitch);
    t_period = c_rate / t_pitch;
    t_block = std::round(t_period);

}

int Changer::left(const int &i) const { return i; }
int Changer::mid(const int &i) const { return i + p_max; }
int Changer::right(const int &i) const { return i + c_size - 1; }

}

