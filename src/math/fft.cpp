#include "math/fft.h"
#include "math/constants.h"

#include <algorithm>
#include <cassert>

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

    if(bits > fftPrecalc.B) return;
    if(1u<<bits != n){
        vector<complex<float> > w(n);
        for(unsigned i=0; i<n; i++) w[i] = v[i];
        w = bluestein(w, inv);
        for(unsigned i=0; i<n; i++) v[i] = w[i];
        return;
    }
    
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
    in_place_fft(f.data(), n);
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
        unsigned j = i == 0 ? 0 : cz-i;
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

vector<float> correlation(const vector<float> &a, const vector<float> &b, unsigned size){

    unsigned n = size;
    if(!n) n = a.size() + b.size() - 1;
    else assert(n >= a.size() && n >= b.size());

    unsigned z = 1;
    while(z < n) z *= 2;

    vector<complex<float> > c(z, 0.0f);
    for(unsigned i=0; i<a.size(); i++) c[i].real(a[i]);
    for(unsigned i=0; i<b.size(); i++) c[i].imag(b[i]);

    in_place_correlation(c);

    vector<float> realc(n);
    for(unsigned i=0; i<n; i++) realc[i] = c[i].real();

    return realc;
}

vector<float> mse(const vector<float> &a, const vector<float> &b){

    assert(a.size() == b.size());
   
    int n = a.size();
    auto c = correlation(a, b, 2*n);
    c.resize(n+1);

    double asum = 0, bsum = 0;
    for(float i : a) asum += i*i;
    for(float i : b) bsum += i*i;

    for(int i=0; i<n; i++){
        c[i] = std::max(0.0, asum + bsum - 2.0 * c[i]);
        asum -= a[i]*a[i];
        bsum -= b[n-i-1]*b[n-i-1];
    } c[n] = 0.0f;

    return c;
}

vector<float> nmse(const vector<float> &a, const vector<float> &b){
    
    auto c = mse(a, b);
    int n = c.size();
    
    double sum = 1e-9;
    for(float i : c) sum += i;
    sum /= n;

    for(int i=n-1; i>=0; i--){
        sum += c[i];
        c[i] /= sum;
    };
   
    return c;
}

vector<float> emse(const vector<float> &a, const vector<float> &b){

    assert(a.size() == b.size());
   
    int n = a.size();
    auto c = correlation(a, b, 2*n);
    c.resize(n+1);

    double a2 = 0, b2 = 0;
    for(int i=n-1; i>=0; i--){
        a2 += a[i]*a[i];
        b2 += b[n-1-i]*b[n-1-i];
        c[i] = std::max(1e-7, a2 + b2 - 2.0 * c[i]);
        c[i] /= std::max(1e-7, a2 + b2);
    } c[n] = 1.0f;
    
    reverse(c.begin(), c.end());

    return c;
}

void in_place_correlation(vector<complex<float> > &v){
    
    int n = v.size();
    assert((n&-n) == n);
    if(n < 2) return;

    for(int i=1, j=n-1; i<j; i++, j--){
        float tmp = v[i].imag();
        v[i].imag(v[j].imag());
        v[j].imag(tmp);
    }

    in_place_fft(v);

    v[0] = v[0].real()*v[0].imag();
    v[n/2] = v[n/2].real()*v[n/2].imag();
    for(int i=1, j=n-1; i<j; i++, j--){
        v[i] = (v[i]-conj(v[j]))*(v[i]+conj(v[j]));
        v[i] = {v[i].imag()*0.25f, -0.25f*v[i].real()};
        v[j] = conj(v[i]);
    }

    in_place_fft(v, 1);
}

std::array<vector<float>, 2> autocorrelation(vector<float> a, vector<float> b){
    
    unsigned n = a.size(), m = b.size(), z = 1;
    while(z < std::max(n, m)) z *= 2;

    vector<complex<float> > c(2*z, 0.0f);
    for(unsigned i=0; i<n; i++) c[i] = {a[i], 0.0f};
    for(unsigned i=0; i<m; i++) c[i] = {c[i].real(), b[i]};

    in_place_fft(c);

    for(unsigned i=0; i<=z; i++){
        
        unsigned j = i == 0 ? 0 : 2*z-i;
        
        float xr = c[i].real()+c[j].real(), xi = c[i].imag()-c[j].imag();
        float yr = c[i].real()-c[j].real(), yi = c[i].imag()+c[j].imag();
        c[i] = c[j] = {(xr*xr + xi*xi) * 0.25f, (yr*yr + yi*yi) * 0.25f};
        
        /*
        auto l = c[i]+c[j];
        auto r = c[i]-c[j];
        complex<float> x{l.real()/2, r.imag()/2};
        complex<float> y{r.real()/2, l.imag()/2};
        c[i] = c[j] = x*conj(x) - y*complex<float>(-y.real(), y.imag()) * complex<float>(0, 1);
        */
    }
    
    in_place_fft(c, 1);

    for(unsigned i=0; i<n; i++) a[i] = c[i].real();
    for(unsigned i=0; i<m; i++) b[i] = c[i].imag();

    return {a, b};
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

