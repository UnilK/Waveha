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
		void fft(std::vector<std::complex<float> > &v, bool inv=0);

		// return DFT of vector v.
		std::vector<std::complex<float> > dft(std::vector<float> &v);
		
		// return inverse DFT of vector cv.
		std::vector<float> idft(std::vector<std::complex<float> > &cv);

		/*
		   calculate the convolution of vectors x and y.
		   n -> fixed size of the resulting vector, if n=0 then it defaults to x.size()+y.size()-1
		   revx -> reverse vector x before convolution
		   revy -> reverse vector y before convolution
		*/
		std::vector<float> convolution(
				std::vector<float> &x, std::vector<float> &y,
				uint32_t n=0, bool revx=0, bool revy=0);
		
};

#endif
