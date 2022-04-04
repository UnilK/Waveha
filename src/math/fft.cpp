#include "math/fft.h"
#include "math/constants.h"

#include <algorithm>
#include <assert.h>

namespace math {

using std::vector;
using std::complex;

FFTPrecalc::FFTPrecalc(unsigned B_){
    resize(B_);
}

void FFTPrecalc::resize(unsigned B_){
    
    B = B_;
    w.resize(B);
    invbit.resize(1u<<B, 0);

    w[B-1].resize(1u<<(B-1));
    for(unsigned i=0; i<(1u<<(B-1)); i++) w[B-1][i] = std::polar(1.0f, -PIF*i/(1<<(B-1)));
    if(B>1) invbit[1] = 1;

    for(unsigned b=B-1; b-->0;){
        unsigned n = 1<<b, nn = 1<<(B-b-1);
        w[b].resize(n);
        for(unsigned i=0; i<nn; i++){
            invbit[i] <<= 1;
            invbit[i+nn] = invbit[i]|1;
        }
        for(unsigned i=0; i<n; i++) w[b][i] = w[b+1][2*i];
    }
}

FFTPrecalc fftPrecalc(18);

void in_place_fft(complex<float> *v, unsigned n, bool inv){
    
    unsigned bits = 0;
    while(1u<<bits < n) bits++;

    // only powers of 2 are supported
    assert(1u<<bits == n);
    assert(bits <= fftPrecalc.B);
    
    unsigned shift = fftPrecalc.B-bits;

    for(unsigned i=0; i<n; i++){
        if(i < fftPrecalc.invbit[i]>>shift) std::swap(v[i], v[fftPrecalc.invbit[i]>>shift]);
    }

    for(unsigned r=0; r<bits; r++){
        unsigned rd = 1u<<r;
        for(unsigned i=0; i<n; i+=2*rd){
            for(unsigned j=i; j<i+rd; j++){
                complex<float> tmp = fftPrecalc.w[r][j-i]*v[j+rd];
                v[j+rd] = v[j]-tmp;
                v[j] = v[j]+tmp;
            }
        }
    }

    if(inv){
        std::reverse(v+1, v+n);
        for(unsigned i=0; i<n; i++) v[i] /= n;
    }
}

void in_place_fft(vector<complex<float> > &v, bool inv){
    in_place_fft(v.data(), v.size(), inv);
}

vector<complex<float> > fft(const float *v, unsigned n){
    vector<complex<float> > f(n);
    for(unsigned i=0; i<n; i++) f[i] = v[i];
    in_place_fft(f, n);
    return f;
}

vector<complex<float> > fft(const vector<float> &v){
    return fft(v.data(), v.size());
}

vector<float> inverse_fft(const complex<float> *v, unsigned n){
    vector<float> r(n);
    vector<complex<float> > f(n);
    for(unsigned i=0; i<n; i++) f[i] = v[i];
    in_place_fft(f, 1);
    for(unsigned i=0; i<n; i++) r[i] = f[i].real();
    return r;
}

vector<float> inverse_fft(const vector<complex<float> > &v){
    return inverse_fft(v.data(), v.size());
}

vector<complex<float> > fft(vector<complex<float> > v, bool inv){
    in_place_fft(v.data(), v.size(), inv);
    return v;
}

vector<complex<float> > fft(complex<float> *v, unsigned n, bool inv){
    vector<complex<float> > f(n);
    for(unsigned i=0; i<n; i++) f[i] = v[i];
    in_place_fft(f, inv);
    return f;
}

vector<float> convolution(vector<float> &a, vector<float> &b, unsigned size){
    
    unsigned za = a.size(), zb = b.size();
    unsigned mx = std::max(za, zb);
    unsigned n = size;
    if(!n) n = za + zb - 1;

    unsigned cz = 1;
    while(cz < n) cz *= 2;

    vector<complex<float> > c(cz, {0, 0});
    
    for(unsigned i=0; i<mx; i++){
        if(i < za) c[i] = {a[i], 0};
        if(i < zb) c[i] = {c[i].real(), b[i]};
    }

    in_place_fft(c);

    for(unsigned i=0; 2*i<=cz; i++){
        unsigned j = (cz-i)%cz;
        c[i] = -(c[i]-conj(c[j]))*(c[i]+conj(c[j]))*complex<float>(0, 0.25f);
        c[j] = conj(c[i]);
    }

    in_place_fft(c, 1);

    vector<float> r(n, 0);
    for(unsigned i=0; i<n; i++) r[i] = c[i].real();
    
    return r;
}

vector<complex<float> > convolution(
        vector<complex<float> > a,
        vector<complex<float> > b,
        unsigned size){

    unsigned n = size;
    if(!n) n = a.size() + b.size() - 1;

    unsigned z = 1;
    while(z < n) z *= 2;

    a.resize(z, {0, 0});
    b.resize(z, {0, 0});

    in_place_fft(a);
    in_place_fft(b);

    for(unsigned i=0; i<z; i++) a[i] *= b[i];

    in_place_fft(a, 1);

    a.resize(n);

    return a;
}

vector<complex<float> > bluestein(vector<complex<float> > v, bool inv){
    
    if(v.empty()) return {};

    unsigned z = v.size(), n = 1;
    while(n < 2*z-1) n *= 2;

    v.resize(n, {0, 0});
    vector<complex<float> > w(n, {0, 0});

    for(unsigned i=0; i<z; i++){
        w[i] = w[(n-i)%n] = std::polar(1.0f, PIF*i*i/z);
        v[i] *= conj(w[i]);
    }

    vector<complex<float> > c = convolution(v, w, n);
 
    for(unsigned i=0; i<z; i++) c[i] *= conj(w[i]);

    c.resize(z);

    if(inv){
        for(auto &i : c) i /= z;
        std::reverse(c.begin()+1, c.end());
    }

    return c;
}

vector<complex<float> > bluestein(vector<float> &v){
    vector<complex<float> > w(v.size());
    for(unsigned i=0; i<v.size(); i++) w[i] = v[i];
    return bluestein(w, 0);
}

vector<float> inverse_bluestein(vector<complex<float> > &v){
    auto w = bluestein(v, 1);
    vector<float> r(w.size());
    for(unsigned i=0; i<v.size(); i++) r[i] = w[i].real();
    return r;
}

}

