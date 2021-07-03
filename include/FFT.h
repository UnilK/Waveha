#ifndef FFT_H
#define FFT_H

#include <cstdint>
#include <vector>
#include <complex>

class FFT{

	protected:

		uint32_t B = 0; // largest precalculated size log 2
		std::vector<std::vector<std::complex<float> > > w;
		std::vector<uint32_t > invbit; // precalculation tables

	public:

		FFT();
		FFT(uint32_t B_);

		void resize_precalc_tables();

		// perform discrete Fourier transform on vector v if inv = 0.
		// perform inverse discrete Fourier transform on vector v if inv = 1.
        // supports only vector sizes of form 2^x, 0<=x<=30. Other sizes will
        // be 0-extended to fit the next power of 2.
		void fft(std::vector<std::complex<float> > &v, bool inv=0);
		void fft(std::complex<float> *v, uint32_t n, bool inv=0);

		// return DFT of vector v.
		std::vector<std::complex<float> > dft(std::vector<float> &v);
		std::complex<float> *dft(float *v, uint32_t n);
		
		// return inverse DFT of vector cv.
        std::vector<float> idft(std::vector<std::complex<float> > &cv);
		float *idft(std::complex<float> *cv, uint32_t n);

		/*
		   calculate the convolution of vectors x and y.
		   n -> fixed size of the resulting vector, if n=0 then it defaults to x.size()+y.size()-1
		   revx -> reverse vector x before convolution
		   revy -> reverse vector y before convolution
		*/
		std::vector<float> convolution(
				std::vector<float> &x, std::vector<float> &y,
				uint32_t n=0, bool revx=0, bool revy=0);
		
        float *convolution(
				float *x, float *y, uint32_t xz, uint32_t yz,
				uint32_t n=0, bool revx=0, bool revy=0);
		
        std::vector<std::complex<float> > convolution(
				std::vector<std::complex<float> > x, std::vector<std::complex<float> > y,
				uint32_t n=0, bool revx=0, bool revy=0);
		
        std::complex<float> *convolution(
				std::complex<float> *x, std::complex<float> *y, uint32_t xz, uint32_t yz,
				uint32_t n=0, bool revx=0, bool revy=0);
		
};

#endif
