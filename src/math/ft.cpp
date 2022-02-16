#include "math/ft.h"
#include "math/constants.h"

#include <math.h>

namespace math {

using std::vector;
using std::complex;

FTPrecalc::FTPrecalc(){
    for(unsigned i=0; i<N; i++){
        exp[i] = std::polar(1.0f, 2.0f*PIF*i/N);
    }
}

FTPrecalc ftPrecalc;

complex<float> get_exp(double num){
    return ftPrecalc.exp[(unsigned)std::floor((num-std::floor(num))*ftPrecalc.N)];
}


complex<float> extract_frequency(float *waves, unsigned size, double frequency){
    
    complex<float> sinusoid = {0, 0};

    for(unsigned i=0; i<size; i++){
        sinusoid += waves[i]*get_exp(frequency*i);
    }

    return std::conj(sinusoid);
}

complex<float> extract_frequency(vector<float> &waves, double frequency){
    return extract_frequency(waves.data(), waves.size(), frequency);
}

}

