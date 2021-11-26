#pragma once

#include <cstdint>
#include <complex>
#include <vector>

namespace math{

using std::vector;
using std::complex;

// calculate discrete fourier transform of v. only sizes
// that are powers of 2 are supported.

void in_place_fft(complex<float> *v, int32_t n, bool inv = 0);
void in_place_fft(vector<complex<float> > &v, bool inv = 0);

vector<complex<float> > fft(float *v, int32_t n);
vector<complex<float> > fft(vector<float> &v);

vector<float> inverse_fft(complex<float> *v, int32_t n);
vector<float> inverse_fft(vector<complex<float> > &v);

vector<complex<float> > fft(vector<complex<float> > v, bool inv = 0);
vector<complex<float> > fft(complex<float> *v, int32_t n, bool inv = 0);

vector<float> convolution(vector<float> &a, vector<float> &b);
vector<complex<float> > convolution(vector<complex<float> > a, vector<complex<float> > b);

}

