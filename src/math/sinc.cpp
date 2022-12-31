#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#include "math/sinc.h"
#include "math/constants.h"

#include <cmath>
#include <iostream>
#include <cassert>

namespace math {

SINCPrecalc sincPrecalc;

std::vector<float> resample(
        const std::vector<float> &in,
        double factor,
        int zeros){
   
    int n = in.size();
    int m = std::floor(n * factor);
    
    std::vector<float> out(m, 0.0f);
    double ifactor = 1.0 / factor;
    double dzeros = zeros;

    for(int i=0; i<n; i++){
        
        int l = std::floor(i * factor), r = std::ceil(i * factor);
        if(l == r) r++;

        l = std::min(m-1, l);
        double ll = l*ifactor, rr = r*ifactor;

        for(;l >= 0 && i - ll <= dzeros; l--, ll-=ifactor) out[l] += in[i] * sinc(i - ll);
        for(;r < m && rr - i <= dzeros; r++, rr+=ifactor) out[r] += in[i] * sinc(rr - i);
    }

    return out;
}

std::vector<float> color_shift(
        const std::vector<float> &in,
        double factor, double interval,
        int zeros){
    
    int n = in.size();
    int m = std::ceil(interval * 2);

    std::vector<float> out(n), window(m);

    for(int i=0; i<m; i++){
        double x = std::cos(PI*i/m);
        window[i] = (1.0 - x*x*x) * 0.5;
    }

    auto stretch = [&](int i) -> void {

    };

    int mid = n / 2;

    return in;
}

SINCPrecalc::SINCPrecalc(){
   
    fsinc.resize(ZN+1);

    fsinc[0] = 1.0f;
    fsinc[ZN] = 0.0f;

    for(unsigned i=1; i<ZN; i++) fsinc[i] = std::sin(PI*i/N) / (PI*i/N);
}

float sinc(float x){
    const unsigned px = x * sincPrecalc.N;
    return px > sincPrecalc.ZN ? 0.0f : sincPrecalc.fsinc[px];
}

float sinc(double x){
    const unsigned px = x * sincPrecalc.N;
    return px > sincPrecalc.ZN ? 0.0f : sincPrecalc.fsinc[px];
}

}

