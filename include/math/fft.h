#pragma once

#include <cstdint>
#include <complex>
#include <vector>

namespace math {

using std::vector;
using std::complex;

// calculate discrete fourier transform of v. only sizes
// that are powers of 2 are supported.

void in_place_fft(complex<float> *v, int32_t n, bool inv = 0);
void in_place_fft(vector<complex<float> > &v, bool inv = 0);

vector<complex<float> > fft(const float *v, int32_t n);
vector<complex<float> > fft(const vector<float> &v);

vector<float> inverse_fft(const complex<float> *v, int32_t n);
vector<float> inverse_fft(const vector<complex<float> > &v);

vector<complex<float> > fft(vector<complex<float> > v, bool inv = 0);
vector<complex<float> > fft(complex<float> *v, int32_t n, bool inv = 0);

vector<float> convolution(vector<float> &a, vector<float> &b, int32_t size = 0);
vector<complex<float> > convolution(
        vector<complex<float> > a,
        vector<complex<float> > b,
        int32_t size = 0);

// these are here for fun
vector<complex<float> > bluestein(vector<complex<float> > v, bool inv = 0); 
vector<complex<float> > bluestein(vector<float> &v, bool inv = 0);
vector<float> inverse_bluestein(vector<complex<float> > &v); 

}

