#pragma once

#include <cstdint>
#include <complex>
#include <vector>

namespace math {

using std::vector;
using std::complex;

// calculate discrete fourier transform of v. only sizes
// that are powers of 2 are supported.

void in_place_fft(complex<float> *v, unsigned n, bool inv = 0);
void in_place_fft(vector<complex<float> > &v, bool inv = 0);

vector<complex<float> > fft(const float *v, unsigned n);
vector<complex<float> > fft(const vector<float> &v);

vector<float> inverse_fft(const complex<float> *v, unsigned n);
vector<float> inverse_fft(const vector<complex<float> > &v);

vector<complex<float> > fft(vector<complex<float> > v, bool inv = 0);
vector<complex<float> > fft(complex<float> *v, unsigned n, bool inv = 0);

vector<float> convolution(vector<float> &a, vector<float> &b, unsigned size = 0);
vector<complex<float> > convolution(
        vector<complex<float> > a,
        vector<complex<float> > b,
        unsigned size = 0);

// these are here for fun
vector<complex<float> > bluestein(vector<complex<float> > v, bool inv = 0); 
vector<complex<float> > bluestein(vector<float> &v, bool inv = 0);
vector<float> inverse_bluestein(vector<complex<float> > &v); 

// default precalc table size is 18 -> vectors of size 2^18 max can be processed

struct FFTPrecalc {

    FFTPrecalc(unsigned);
    void resize(unsigned);

    unsigned B;

    vector<vector<complex<float> > > w;
    vector<unsigned> invbit;

};

extern FFTPrecalc fftPrecalc;

}

