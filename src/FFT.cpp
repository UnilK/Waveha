#include <algorithm>

#include "FFT.h"

const float PI = 3.14159265358979323; // more that enough digits.

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

		uint32_t z = 1<<this->w.size(), b = this->w.size();
		
		this->w.resize(this->w.size()+1);
		this->w[b].resize(z);

		for(uint32_t i=0; i<z; i++) this->w[b][i] = std::polar(1.0f, PI*i/z);

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

	v.resize(1<<b, {0, 0});
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

std::vector<std::complex<float> > FFT::dft(std::vector<float> &v){
	std::vector<std::complex<float> > cv(v.size());
	for(uint32_t i=0; i<v.size(); i++) cv[i] = {v[i], 0};
	this->fft(cv, 0);
	return cv;
}

std::vector<float> FFT::idft(std::vector<std::complex<float> > &cv){
	this->fft(cv, 1);
	std::vector<float> v(cv.size());
	for(uint32_t i=0; i<cv.size(); i++) v[i] = cv[i].real();
	return v;
}

std::vector<float> FFT::convolution(
		std::vector<float> &x, std::vector<float> &y,
		uint32_t n, bool revx, bool revy){
	
	uint32_t b = 0, zx = x.size(), zy = y.size();
	if(!n) n = zx+zy-1;
	
	while((uint32_t)1<<b < std::max(n, std::max(zx, zy))) b++;	

	std::vector<std::complex<float> > cx(1<<b, {0, 0}), cy(1<<b, {0, 0});

	if(revx) for(uint32_t i=zx-1; i>=0; i--) cx[zx-1-i] = {x[i], 0};
	else for(uint32_t i=0; i<zx; i++) cx[i] = {x[i], 0};
	
	if(revy) for(uint32_t i=zy-1; i>=0; i--) cy[zy-1-i] = {y[i], 0};
	else for(uint32_t i=0; i<zy; i++) cy[i] = {y[i], 0};

	this->fft(cx, 0);
	this->fft(cy, 0);

	// calculating convolution
	for(uint32_t i=0; i<(uint32_t)1<<b; i++) cx[i] *= cy[i];

	// inverse
	this->fft(cx, 1);
	std::vector<float> xy(n);
	for(uint32_t i=0; i<n; i++) xy[i] = cx[i].real();

	return xy;
}


