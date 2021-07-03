#ifndef FT_H
#define FT_H

#include <cstdint>
#include <complex>

class FT{
    
    protected:

        float dataFrequency = 44100;

    public:

        FT();
        FT(float dataFrequency_);
        
        std::complex<float> roll(float *waves, int32_t size, float frequency);

        float calc_match(float *waves, int32_t size, float fundamental);

};

#endif
