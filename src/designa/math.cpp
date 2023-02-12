#include "designa/math.h"

#include <algorithm>
#include <cassert>
#include <random>

namespace designa {

using std::vector;
using std::complex;
using std::array;

float rnd(float d){
    static std::mt19937 rng32;
    return std::uniform_real_distribution<float>(-d, d)(rng32);
}

void in_place_fft(vector<complex<float> > &v, bool inv){

    int n = v.size();

    int bits = 0;
    while(1ll<<bits < n) bits++;
    assert(1ll<<bits == n && bits <= 30);
    if(bits == 0) return;

    // dynamic precalculation tables, calculated in O(n)

    static int N = -1;
    static vector<int> invbit;
    static vector<vector<complex<float> > > w;

    if(bits > N){
        
        N = bits;
        int M = bits-1;

        w.resize(N);
        invbit = vector<int>(1<<N, 0);

        w[M].resize(1<<M);
        for(int i=0; i<(1<<M); i++) w[M][i] = std::polar<double>(1.0, -M_PI * i / (1<<M));
        
        invbit[1] = 1;
        for(int b=M-1; b>=0; b--){

            int n = 1<<b, nn = 1<<(M-b);
            w[b].resize(n);
            
            for(int i=0; i<nn; i++){
                invbit[i] <<= 1;
                invbit[i+nn] = invbit[i] | 1;
            }

            for(int i=0; i<n; i++) w[b][i] = w[b+1][2*i];
        }
    }
    
    // actual fft
    
    int shift = N - bits;

    for(int i=0; i<n; i++)
        if(i < invbit[i]>>shift) std::swap(v[i], v[invbit[i]>>shift]);

    for(int r=0, rd=1; r<bits; r++, rd*=2){
        for(int i=0; i<n; i+=2*rd){
            for(int j=i; j<i+rd; j++){
                complex<float> wv = w[r][j-i] * v[j+rd];
                v[j+rd] = v[j] - wv;
                v[j] = v[j] + wv;
            }
        }
    }

    // inverse

    if(inv){
        std::reverse(v.begin()+1, v.end());
        float invn = 1.0 / n; // accurate since n is a power of 2
        for(int i=0; i<n; i++) v[i] *= invn;
    }
}

vector<complex<float> > fft(const vector<float> &v){
    vector<complex<float> > w(v.size());
    for(unsigned i=0; i<v.size(); i++) w[i] = v[i];
    in_place_fft(w, 0);
    return w;
}

array<vector<complex<float> >, 2> fft(const vector<float> &a, const vector<float> &b){
    
    assert(a.size() == b.size());

    int n = a.size();
    vector<complex<float> > c(n), fa(n), fb(n);
    for(int i=0; i<n; i++) c[i] = {a[i], b[i]};

    in_place_fft(c);

    for(int i=0; i<=n-i; i++){
        int j = (n-i)&(n-1);
        fa[i] = 0.5f * (c[i] + conj(c[j]));
        fb[i] = 0.5f * (c[i] - conj(c[j]));
        fb[i] = {fb[i].imag(), -fb[i].real()};
        fa[j] = conj(fa[i]);
        fb[j] = conj(fb[i]);
    }

    return {fa, fb};
}

vector<float> inverse_fft(const vector<complex<float> > &v){
    auto w = v;
    in_place_fft(w);
    vector<float> r(w.size());
    for(unsigned i=0; i<v.size(); i++) r[i] = w[i].real();
    return r;
}

vector<float> energy_mse(const vector<float> &a, const vector<float> &b){
    
    int n = a.size();
    
    assert(a.size() == b.size());
    
    int m = 1;
    while(m < n) m *= 2;

    vector<complex<float> > cc(2*m, 0.0f);
    for(int i=0; i<n; i++) cc[i].real(a[i]);
    cc[0].imag(b[0]);
    for(int i=1; i<n; i++) cc[2*m-i].imag(b[i]);

    in_place_fft(cc);
    
    cc[0] = cc[0].real()*cc[0].imag();
    cc[m] = cc[m].real()*cc[m].imag();
    for(int i=1, j=2*m-1; i<j; i++, j--){
        cc[i] = (cc[i]-conj(cc[j]))*(cc[i]+conj(cc[j]));
        cc[i] = {cc[i].imag()*0.25f, -0.25f*cc[i].real()};
        cc[j] = conj(cc[i]);
    }

    in_place_fft(cc, 1);

    vector<float> c(n+1);
    for(int i=0; i<=n; i++) c[i] = cc[i].real();

    double a2 = 0, b2 = 0;
    for(int i=n-1; i>=0; i--){
        a2 += a[i]*a[i];
        b2 += b[n-1-i]*b[n-1-i];
        c[i] = std::max(1e-7, a2 + b2 - 2.0 * c[i]);
        c[i] /= std::max(1e-7, a2 + b2);
    } c[n] = 0.0f;
    
    reverse(c.begin(), c.end());

    return c;
}

}


