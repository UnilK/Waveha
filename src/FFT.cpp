#include <algorithm>
#include <string.h>

#include "FFT.h"
#include "mathConstants.h"

FFT::FFT(){
	this->B = 0;
	this->resize_precalc_tables();
}

FFT::FFT(uint32_t B_){
	this->B = std::max((uint32_t)30, B_);
	this->resize_precalc_tables();
}

void FFT::resize_precalc_tables(){
			
	while(this->w.size() <= B){
				
		/*

		   precalculate w[b][x] = e^(i*PI*x/(2^b))

		   and the invbit array:
		   examples in base 2 of the invbit array:

		   invbit[1000111] = 1110001
		   invbit[0] = 0
		   invbit[10110111] = 11101101

		   it reverses the bit order.

		*/

		uint32_t z = (uint32_t)1<<this->w.size(), b = this->w.size();
		
		this->w.resize(this->w.size()+1);
		this->w[b].resize(z);

		for(uint32_t i=0; i<z; i++) this->w[b][i] = std::polar(1.0f, PIF*i/z);

		this->invbit.resize(z, 0);
		for(uint32_t i=0; i<z/2; i++){
			this->invbit[i] <<= 1;
			this->invbit[i+z/2] = this->invbit[i]+1;
		}
	}

}

void FFT::fft(std::vector<std::complex<float> > &v, bool inv){
	
	/*
	   let's examine how the recursive FFT changes
	   the ordering of the input array:
	   
	   [000, 001, 010, 011, 100, 101, 110, 111]
	   [000, 010, 100, 110], [001, 011, 101, 111]
	   [000, 100], [010, 110], [001, 101], [011, 111]
	   [000], [100], [010], [110], [001], [101], [011], [111]

	   [000, 100, 010, 110, 001, 101, 011, 111]
	  
	   The index of element x in the resulting array
	   is it's index in the inupt array, but with the binary
	   representation flipped. This makes sense, since
	   on each layer we sort the array by the least
	   significant bit.

	   Just reorder the array, then run the basic
	   FFT algorithm on a bottom-to-top loop such that the results
	   are calculated in the indices used by the next layer.

	   O(1) extra memory used with O(N*log(N)) preclac,
	   O(N*log(N)) time with a good constant compared to
	   the recursive implementation.
	*/

	uint32_t b = 0;
	while((uint32_t)1<<b < v.size()) b++;

	this->B = std::max(b, this->B);
	this->resize_precalc_tables();

	v.resize((uint32_t)1<<b, {0, 0});
	uint32_t n = v.size();

	// the invbit array for b-1 is the same as for b,
	// but each element is shifted back by 1 bit.
	uint32_t shift = B-b;

	for(uint32_t i=0; i<n; i++){
		if(i < this->invbit[i]>>shift) std::swap(v[i], v[this->invbit[i]>>shift]);
	}

	for(uint32_t r=0; r<b; r++){
		uint32_t rd = 1<<r;
		for(uint32_t i=0; i<n; i+=2*rd){
			for(uint32_t j=i; j<i+rd; j++){
				std::complex<float> tmp = w[r][j-i]*v[j+rd];
				v[j+rd] = v[j]-tmp;
				v[j] = v[j]+tmp;
			}
		}
	}

	if(inv){
		std::reverse(v.begin()+1, v.end());
		for(auto &i : v) i /= n;
	}
}

void FFT::fft(std::complex<float> *v, uint32_t n, bool inv){
	
	uint32_t b = 0;
	while((uint32_t)1<<b < n) b++;

	this->B = std::max(b, this->B);
	this->resize_precalc_tables();

    if((uint32_t)1<<b != n) return;

	uint32_t shift = B-b;

	for(uint32_t i=0; i<n; i++){
		if(i < this->invbit[i]>>shift) std::swap(v[i], v[this->invbit[i]>>shift]);
	}

	for(uint32_t r=0; r<b; r++){
		uint32_t rd = 1<<r;
		for(uint32_t i=0; i<n; i+=2*rd){
			for(uint32_t j=i; j<i+rd; j++){
				std::complex<float> tmp = w[r][j-i]*v[j+rd];
				v[j+rd] = v[j]-tmp;
				v[j] = v[j]+tmp;
			}
		}
	}

	if(inv){
		std::reverse(v+1, v+n);
		for(uint32_t i=0; i<n; i++) v[i] /= n;
	}
}


std::vector<std::complex<float> > FFT::dft(std::vector<float> &v){
	std::vector<std::complex<float> > cv(v.size());
	for(uint32_t i=0; i<v.size(); i++) cv[i] = {v[i], 0};
	this->fft(cv, 0);
	return cv;
}

std::complex<float> *FFT::dft(float *v, uint32_t n){
	std::complex<float> *cv = new std::complex<float>[n];
	for(uint32_t i=0; i<n; i++) cv[i] = {v[i], 0};
	this->fft(cv, n, 0);
	return cv;
}


std::vector<float> FFT::idft(std::vector<std::complex<float> > &cv){
	this->fft(cv, 1);
	std::vector<float> v(cv.size());
	for(uint32_t i=0; i<cv.size(); i++) v[i] = cv[i].real();
	return v;
}

float *FFT::idft(std::complex<float> *cv, uint32_t n){
	this->fft(cv, n, 1);
	float *v = new float[n];
	for(uint32_t i=0; i<n; i++) v[i] = cv[i].real();
	return v;
}


std::vector<float> FFT::convolution(
		std::vector<float> &x, std::vector<float> &y,
		uint32_t n, bool revx, bool revy){
	
	uint32_t b = 0, zx = x.size(), zy = y.size();
	if(!n) n = zx+zy-1;
	
	while((uint32_t)1<<b < std::max(n, std::max(zx, zy))) b++;	

	std::vector<std::complex<float> > cx(1<<b, {0, 0}), cy(1<<b, {0, 0});

	if(revx) for(uint32_t i=0; i<zx; i++) cx[i] = {x[zx-i-1], 0};
	else for(uint32_t i=0; i<zx; i++) cx[i] = {x[i], 0};
	
	if(revy) for(uint32_t i=0; i<zy; i++) cy[i] = {y[zy-i-1], 0};
	else for(uint32_t i=0; i<zy; i++) cy[i] = {y[i], 0};

	this->fft(cx, 0);
	this->fft(cy, 0);

	// calculating convolution
	for(uint32_t i=0; i<(uint32_t)1<<b; i++) cx[i] *= cy[i];

	// inverse
	this->fft(cx, 1);
	std::vector<float> xy(n, 0);
	for(uint32_t i=0; i<n; i++) xy[i] = cx[i].real();

	return xy;
}

float *FFT::convolution(
		float *x, float *y, uint32_t zx, uint32_t zy,
		uint32_t n, bool revx, bool revy){
	
	uint32_t b = 0;
	if(!n) n = zx+zy-1;
	
	while((uint32_t)1<<b < std::max(n, std::max(zx, zy))) b++;

	std::complex<float> *cx = new std::complex<float>[(uint32_t)1<<b]();
	std::complex<float> *cy = new std::complex<float>[(uint32_t)1<<b]();

	if(revx) for(uint32_t i=0; i<zx; i++) cx[i] = {x[zx-i-1], 0};
	else for(uint32_t i=0; i<zx; i++) cx[i] = {x[i], 0};
	
	if(revy) for(uint32_t i=0; i<zy; i++) cy[i] = {y[zy-i-1], 0};
	else for(uint32_t i=0; i<zy; i++) cy[i] = {y[i], 0};

	this->fft(cx, (uint32_t)1<<b, 0);
	this->fft(cy, (uint32_t)1<<b, 0);

	for(uint32_t i=0; i<(uint32_t)1<<b; i++) cx[i] *= cy[i];

	this->fft(cx, (uint32_t)1<<b, 1);
	float *xy = new float[n]();

	for(uint32_t i=0; i<n; i++) xy[i] = cx[i].real();

    delete[] cx;
    delete[] cy;

	return xy;
}

std::vector<std::complex<float> > FFT::convolution(
		std::vector<std::complex<float> > x, std::vector<std::complex<float> > y,
		uint32_t n, bool revx, bool revy){
	
	uint32_t b = 0, zx = x.size(), zy = y.size();
	if(!n) n = zx+zy-1;
	
	while((uint32_t)1<<b < std::max(n, std::max(zx, zy))) b++;	
	
    x.resize(1<<b, {0, 0});
    y.resize(1<<b, {0, 0});

	this->fft(x, 0);
	this->fft(y, 0);

	// calculating convolution
	for(uint32_t i=0; i<(uint32_t)1<<b; i++) x[i] *= y[i];

	// inverse
	this->fft(x, 1);
	std::vector<std::complex<float> > xy(n, {0, 0});
	for(uint32_t i=0; i<n; i++) xy[i] = x[i];

	return xy;
}

std::complex<float> *FFT::convolution(
		std::complex<float> *x, std::complex<float> *y, uint32_t zx, uint32_t zy,
		uint32_t n, bool revx, bool revy){
	
	uint32_t b = 0;
	if(!n) n = zx+zy-1;
	
	while((uint32_t)1<<b < std::max(n, std::max(zx, zy))) b++;

	std::complex<float> *cx = new std::complex<float>[(uint32_t)1<<b]();
	std::complex<float> *cy = new std::complex<float>[(uint32_t)1<<b]();

    memcpy(cx, x, sizeof(std::complex<float>)*zx);
    memcpy(cy, y, sizeof(std::complex<float>)*zy);

    if(revx) std::reverse(cx, cx+zx);
    if(revy) std::reverse(cy, cy+zy);

	this->fft(cx, (uint32_t)1<<b, 0);
	this->fft(cy, (uint32_t)1<<b, 0);

	for(uint32_t i=0; i<(uint32_t)1<<b; i++) cx[i] *= cy[i];

	this->fft(cx, (uint32_t)1<<b, 1);
    std::complex<float> *xy = new std::complex<float>[n](); 
    memcpy(cx, xy, sizeof(std::complex<float>)*n);

    delete[] cx;
    delete[] cy;

	return xy;
}

