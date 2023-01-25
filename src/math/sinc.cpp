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

SINCPrecalc::SINCPrecalc(){
   
    fsinc.resize(ZN+1);

    fsinc[0] = 1.0f;
    fsinc[ZN] = 0.0f;

    for(unsigned i=1; i<ZN; i++) fsinc[i] = std::sin(PI*i/N) / (PI*i/N);
    for(unsigned i=1; i<ZN; i++) fsinc[i] *= 0.5f + 0.5f * std::cos(PI*i/ZN);
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

