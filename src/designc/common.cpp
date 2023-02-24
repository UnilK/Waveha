#include "designc/common.h"

#include <cassert>
#include <cmath>
#include <random>

namespace designc {

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

vector<complex<float> > extract_phase(
        const vector<complex<float> > &frequency)
{
    vector<complex<float> > phase(frequency.size());
    for(unsigned i=0; i<phase.size(); i++) phase[i] = unit_complex(frequency[i]);
    return phase;
}

vector<float> extract_energy(
        const vector<complex<float> > &frequency)
{    
    vector<float> energy(frequency.size());
    for(unsigned i=0; i<energy.size(); i++) energy[i] = std::norm(frequency[i]);
    return energy;
}

vector<complex<float> > create_frequency(
        const vector<float> &energy,
        vector<complex<float> > phase)
{
    assert(energy.size() == phase.size());

    for(unsigned i=0; i<phase.size(); i++) phase[i] *= std::sqrt(energy[i]);
    return phase;
}

vector<float> shift_bins(
        const std::vector<float> &bin,
        const std::vector<float> &shift)
{
    assert(bin.size() == shift.size());

    int n = bin.size();
    std::vector<float> result(n, 0.0f);

    result[0] = bin[0];

    for(int i=0; i<n; i++){
        
        const float j = i * shift[i];
        const float width = std::max(1.0f, shift[i]);
        const float w = M_PI / width;

        const int l = std::max<int>(1, std::ceil(j - width));
        const int r = std::min<int>(n-1, std::floor(j + width));

        float sum = 0.0f;
        for(int k=l; k<=r; k++) sum += 0.5f + 0.5f * std::cos(w * (j - k));
    
        sum = 1.0f / (sum + 1e-18f);
        
        for(int k=l; k<=r; k++)
            result[k] += (0.5f + 0.5f * std::cos(w * (j - k))) * sum * bin[i];
    }

    return result;
}

}

