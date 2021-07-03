#ifndef SINUSOIDFT_H
#define SINUSOIDFT_H

#include <complex>
#include <vector>

class SinusoidFT{
   
    /*
       Class for calculating all sorts of stuff
       about the Fourier transform of a
       generic sinusoid sin(f*x+w)*A

       f = frequency
       w = initial phase (in radians)
       A = amplitude

       one-cycle FT of a... complex sinusoid? A*e^(i*f+w) is what I mean.
       FT(x) = A*e^(i*w)*(-i/(f+x))*(e^(2*pi*i*(f+x))-1), x != f
       and FT(f) = A*2*pi*e^(i*w)

       translating this for a DFT, we get
       DFT(x) = A*e^(i*w)*(1-e^(i*2*pi*(f+x)))/(1-e^(i*2*pi*(f+x)/N)), x != f
       and DFT(f) = A*N*e^(i*w), where N is the size of the array being transformed.

       Then, a sinusoid is:
       SinusoidFT(x) = (DFT(x) + conj(DFT(-x))) / 2

   */

    protected:

        // sampling rate of the input data and input data size.
        int32_t dataFrequency = 1, n = 1;
        float f = 0, w = 0, a = 0;
        
        void set_params(float f_, float w_, float a_, int32_t n_);
        std::complex<float> get(float x);
        std::complex<float> inv_get(float x);

        std::complex<float> get_sinusoid(std::complex<float> left, std::complex<float> right);
        float evaluate_sinusoid(std::complex<float> left, std::complex<float> right);

    public:

        SinusoidFT();
        SinusoidFT(int32_t dataFrequency_);

        void set_frequency(int32_t dataFrequency_);

        std::complex<float> get(float x,
                float frequency, float phase, float amplitude, int32_t size);

        void add_sinusoid(std::vector<std::complex<float> > &spectrum,
                float frequency, float phase, float amplitude);
        void add_sinusoid(std::complex<float> *spectrum, int32_t size,
                float frequency, float phase, float amplitude);
        
        // get the phase & amplitude of requency in the spectrum.
        std::complex<float> get_sinusoid(
                std::vector<std::complex<float> > &spectrum, float frequency);
        std::complex<float> get_sinusoid(
                std::complex<float> *spectrum, int32_t size, float frequency);
        
        // evaluate a number that somewhat correlates to the amplitude of the sinusoid
        float evaluate_sinusoid(
                std::vector<std::complex<float> > &spectrum, float frequency);
        float evaluate_sinusoid(
                std::complex<float> *spectrum, int32_t size, float frequency);

};

#endif
