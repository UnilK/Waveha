#include "designb/common.h"

#include <cassert>
#include <cmath>
#include <random>

namespace designb {

using std::vector;
using std::complex;

float rnd(const float &d){
    static std::mt19937 rng32;
    return std::uniform_real_distribution<float>(-d, d)(rng32);
}

complex<float> unit_complex(const complex<float> &c){
    return c / (std::abs(c) + 1e-18f);
}

vector<float> cos_window(int length){

    double a = 2 * M_PI / length;

    vector<float> window(length);
    for(int i=0; i<length; i++) window[i] = 1.0f - std::cos(a * i);

    return window;
}

vector<complex<float> > cos_wavelet(int length, double spins){
    
    double a = 2 * M_PI / length;
    double b = 2 * M_PI * spins / length;
    vector<complex<float> > w(length);
    
    for(int i=0; i<length; i++)
        w[i] = std::polar<double>(0.5f - 0.5f * std::cos(a * i), b * i);

    return w;
}

float maximum_peak(const vector<float> &v, int min, int max){

    assert(min > 0);
    assert(max < (int)v.size());
    assert(max - min > 1);

    int j = (max + min) / 2;

    for(int i=min+1; i+1<=max; i++)
        if(v[i] > v[j] && v[i] > v[i-1] && v[i] > v[i+1]) j = i;

    auto para = [](float i, float l, float m, float r) -> float {
        const float a = 0.5f * (r + l) - m;
        const float b = (r - l) * 0.5f;
        const float d = - b / (2 * a);
        return i + d;
    };

    if(v[j] > v[j-1] && v[j] > v[j+1]) return para(j, v[j-1], v[j], v[j+1]);
    return j;
}

}

