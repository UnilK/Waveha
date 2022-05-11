#include "math/ft.h"
#include "math/constants.h"

#include <math.h>

namespace math {

using std::vector;
using std::complex;

complex<float> lt(const float *waves, unsigned size, float frequency){
    
    complex<float> sinusoid = {0, 0};

    for(unsigned i=0; i<size; i++){
        sinusoid += waves[i]*std::polar(1.0f, -2 * PIF / size * i * (float)frequency);
    }

    return std::conj(sinusoid);
}

complex<float> lt(const vector<float> &waves, float frequency){
    return lt(waves.data(), waves.size(), frequency);
}

vector<complex<float> > ft(const float *waves, unsigned size, unsigned n){
    
    vector<complex<float> > frequencies(n, {0, 0});
    
    vector<complex<float> > exp(size);
    for(unsigned i=0; i<size; i++) exp[i] = cexp((double)(size - i) / size);

    for(unsigned i=0; i<size; i++){
        for(unsigned j=0; j<n; j++){
            frequencies[j] += exp[i * (j+1) % size] * waves[i];
        }
    }

    for(auto &i : frequencies) i = 2.0f * i / (float)size;

    return frequencies;
}

vector<complex<float> > ft(const vector<float> &waves, unsigned n){
    return ft(waves.data(), waves.size(), n);
}

vector<float> ift(const vector<complex<float> > &frequencies, unsigned size){
    
    std::vector<float> waves(size, 0);
    
    vector<complex<float> > exp(size);
    for(unsigned i=0; i<size; i++) exp[i] = cexp((double)i / size);

    unsigned n = frequencies.size();

    for(unsigned j=0; j<n; j++){
        for(unsigned i=0; i<size; i++){
            waves[i] += (frequencies[j]*exp[(long unsigned)i * (j+1) % size]).real();
        }
    }

    return waves;
}

FTPrecalc::FTPrecalc(){
    for(unsigned i=0; i<N; i++){
        exp[i] = std::polar(1.0f, 2.0f*PIF*i/N);
    }
    exp[N] = exp[0];
}

FTPrecalc ftPrecalc;

complex<float> cexp(double radians){
    double dummy;
    return ftPrecalc.exp[(unsigned)(std::modf(radians, &dummy)*ftPrecalc.N)];
}

}

