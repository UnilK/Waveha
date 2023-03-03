#include "designe/math.h"

#include <algorithm>
#include <cassert>

namespace designe {

using std::vector;
using std::complex;
using std::array;

void in_place_fft(vector<complex<float> > &v, bool inv){

    int n = v.size();

    int bits = 0;
    while(1ll<<bits < n) bits++;
    assert(1ll<<bits == n && bits < 30);
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

            int x = 1<<b, y = 1<<(M-b);
            w[b].resize(x);
            
            for(int i=0; i<y; i++){
                invbit[i] <<= 1;
                invbit[i+y] = invbit[i] | 1;
            }

            for(int i=0; i<x; i++) w[b][i] = w[b+1][2*i];
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
    
    int n = v.size();
    int m = n/2 + 1;

    vector<complex<float> > w(n);
    for(int i=0; i<n; i++) w[i] = v[i];
    
    in_place_fft(w, 0);
    w.resize(m);
    
    return w;
}

array<vector<complex<float> >, 2> fft(const vector<float> &a, const vector<float> &b){
    
    assert(a.size() == b.size());

    int n = a.size();
    int m = n/2 + 1;

    vector<complex<float> > c(n), fa(m), fb(m);
    for(int i=0; i<n; i++) c[i] = {a[i], b[i]};

    in_place_fft(c, 0);

    for(int i=0; i<m; i++){
        int j = (n-i)&(n-1);
        fa[i] = 0.5f * (c[i] + conj(c[j]));
        fb[i] = 0.5f * (c[i] - conj(c[j]));
        fb[i] = {fb[i].imag(), -fb[i].real()};
    }

    return {fa, fb};
}

vector<float> inverse_fft(const vector<complex<float> > &v){
    
    int m = v.size();
    int n = 2 * m - 2;

    auto w = v;
    w.resize(n);
    
    for(int i=1; i<m; i++) w[n-i] = std::conj(w[i]);

    in_place_fft(w, 1);
    
    vector<float> r(n);
    for(int i=0; i<n; i++) r[i] = w[i].real();
    
    return r;
}

array<vector<float>, 2> inverse_fft(vector<complex<float> > a, vector<complex<float> > b){

    assert(a.size() == b.size());

    int m = a.size();
    int n = 2 * m - 2;
    
    a.resize(n);
    b.resize(n);
    
    for(int i=1; i<m; i++){
        a[n-i] = std::conj(a[i]);  
        b[n-i] = std::conj(b[i]);
    }

    for(int i=0; i<n; i++){
        a[i].real(a[i].real() - b[i].imag());
        a[i].imag(a[i].imag() + b[i].real());
    }

    in_place_fft(a, 1);

    vector<float> ta(n), tb(n);
    for(int i=0; i<n; i++){
        ta[i] = a[i].real();
        tb[i] = a[i].imag();
    }
    
    return {ta, tb};
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

    in_place_fft(cc, 0);
    
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

vector<float> padded_energy_mse(const vector<float> &a, const vector<float> &b, int border){

    auto mse = energy_mse(a, b);
    
    for(int i=0; i+2*border < (int)mse.size(); i++) mse[i] = mse[i+2*border];
    mse.resize(mse.size() - 2 * border);

    return mse;
}

}


