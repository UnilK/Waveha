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

vector<complex<float> > ft(const float *waves, unsigned size, unsigned n, bool haszero){
    
    vector<complex<float> > frequencies(n, 0.0f);
    
    vector<complex<float> > exp(size);
    for(unsigned i=0; i<size; i++) exp[i] = cexp((double)(size - i) / size);

    unsigned offset = !haszero;

    for(unsigned i=0; i<size; i++){
        for(unsigned j=0; j<n; j++){
            frequencies[j] += exp[i * (j+offset) % size] * waves[i];
        }
    }

    for(auto &i : frequencies) i = 2.0f * i / (float)size;

    return frequencies;
}

vector<complex<float> > ft(const vector<float> &waves, unsigned n, bool haszero){
    return ft(waves.data(), waves.size(), n, haszero);
}

vector<complex<float> > cos_window_ft(float *waves, unsigned size, unsigned n, bool haszero){
    
    vector<complex<float> > frequencies(n, 0.0f);
    
    vector<complex<float> > exp(size);
    for(unsigned i=0; i<size; i++) exp[i] = cexp((double)-i / size);

    for(unsigned i=0; i<size; i++) waves[i] *= 0.5f - 0.5f * exp[i].real();

    unsigned offset = !haszero;

    for(unsigned i=0; i<size; i++){
        for(unsigned j=0; j<n; j++){
            frequencies[j] += exp[i * (2*(j+offset)) % size] * waves[i];
        }
    }

    for(auto &i : frequencies) i = 4.0f * i / (float)size;

    return frequencies;
}

vector<complex<float> > cos_window_ft(vector<float> waves, unsigned n, bool haszero){
    return cos_window_ft(waves.data(), waves.size(), n, haszero);
}

vector<complex<float> > precise_ft(const vector<float> &waves,
        unsigned n, bool haszero, float speed){
    
    vector<complex<float> > frequencies(n, 0.0f);

    unsigned size = waves.size();
    unsigned offset = !haszero;

    for(unsigned i=0; i<size; i++){
        for(unsigned j=0; j<n; j++){
            frequencies[j] += waves[i] * std::polar(1.0f, -2 * PIF / size * i * (j + offset) * speed);
        }
    }

    for(auto &i : frequencies) i = 2.0f * speed * i / (float)size;

    return frequencies;
}

vector<float> ift(const vector<complex<float> > &frequencies,
        unsigned size,
        bool haszero){
    
    std::vector<float> waves(size, 0);
    
    vector<complex<float> > exp(size);
    for(unsigned i=0; i<size; i++) exp[i] = cexp((double)i / size);

    unsigned n = frequencies.size();
    
    unsigned offset = !haszero;

    for(unsigned j=0; j<n; j++){
        for(unsigned i=0; i<size; i++){
            waves[i] += (frequencies[j]*exp[i * (j+offset) % size]).real();
        }
    }

    return waves;
}

vector<float> sized_ift(const vector<complex<float> > &frequencies,
        unsigned period,
        unsigned size){
    
    std::vector<float> waves(size, 0);
    
    vector<complex<float> > exp(period);
    for(unsigned i=0; i<period; i++) exp[i] = cexp((double)i / period);

    unsigned n = frequencies.size();
    
    for(unsigned j=0; j<n; j++){
        for(unsigned i=0; i<size; i++){
            waves[i] += (frequencies[j]*exp[i * (j+1) % period]).real();
        }
    }

    return waves;
}

FTPrecalc::FTPrecalc(){
    for(unsigned i=0; i<N; i++){
        exp[i] = std::polar(1.0, 2.0*PI*i/N);
    }
    exp[N] = exp[0];
}

FTPrecalc ftPrecalc;

complex<float> cexp(double radians){
    double dummy;
    double fractional = std::modf(radians, &dummy);
    fractional = fractional < 0.0 ? fractional + 1.0 : fractional;
    return ftPrecalc.exp[(unsigned)(fractional*ftPrecalc.dN)];
}

DFTPrecalc::DFTPrecalc(){
    
    exp.resize(N+1);

    double b15 = 1<<15, i15 = 1.0f / (1<<15);

    for(unsigned i=0; i<N; i++){
        auto x = std::polar(1.0, 2.0*PI*i/N);
        float xr = ((int)(x.real()*b15))*i15;
        float xi = ((int)(x.imag()*b15))*i15;
        exp[i] = {xr, xi};
    }
    exp[N] = exp[0];
}

DFTPrecalc dftPrecalc;

complex<float> dexp(double radians){
    double dummy;
    double fractional = std::modf(radians, &dummy);
    fractional = fractional < 0.0 ? fractional + 1.0 : fractional;
    return dftPrecalc.exp[(unsigned)(fractional*dftPrecalc.dN)];
}

}

