#include "change/resample.h"
#include "math/constants.h"

#include <algorithm>
#include <cmath>
#include <complex>

namespace change {

std::vector<float> resample(const std::vector<float> &audio, double shift, int size){
    
    using std::vector;
    using std::complex;

    size = std::max(size / 4, 1) * 4;
    
    int n = audio.size();
    int m = std::ceil(n/shift);
    shift = (double)n/m;

    vector<float> output(m, 0.0f), window(size);
    for(int i=0; i<size; i++) window[i] = (1.0 - std::cos(2.0 * PI * i / size)) / std::sqrt(6);
    
    int freqs = size / 2 + 1;
    vector<vector<complex<float> > > w(freqs, vector<complex<float> >(size));
    for(int i=0; i<freqs; i++){
        for(int j=0; j<size; j++) w[i][j] = std::polar(1.0, -2.0 * PI * j * i / size);
    }

    vector<complex<float> > f(freqs);

    const double scalar = (2.0 / std::sqrt(6.0)) / size;
   
    double p = 0.0;
    for(int in=0, out=0; in<n; in+=size/4){
        
        while(p < in){ p += shift; out++; }

        for(auto &i : f) i = 0.0f;
        
        for(int i=0; i<freqs; i++){
            for(int j=0; j<size && in+j < n; j++){
                f[i] += w[i][j] * window[j] * audio[in + j] ;
            }
        }

        int j = 0, k = 0;
        double d = p - in;

        while(d < size && out+k < m){
            
            while(j+1 < d) j++;
            
            const complex<float> rot = std::polar(1.0f, 2 * PIF * (float)(d-j) / size);
            complex<float> ang = rot;
            
            float sum = f[0].real() / 2;
            for(int i=1; i+1<freqs; i++){
                sum += (f[i] * std::conj(w[i][j]) * ang).real();
                ang *= rot;
            }
            if(freqs > 1) sum += f[freqs-1].real() / 2;
            
            sum *= scalar * (1.0 - std::cos(2 * PI * d / size));
            output[out+(k++)] += sum;
            d += shift;
        }
    }

    return output;
}

}

