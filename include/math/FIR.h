#ifndef FIR_H
#define FIR_H

#include <cstdint>
#include <vector>

class FIR{
    
    // template class for Finite Impulse Responses.

    protected:

        // the "quality" of the impulse i.e. how long the impulse is simulated,
        // to each side.
        int32_t length = 0;

    public:

        FIR();
        FIR(int32_t length_);

        // evaluate function 
        virtual float get(double num) = 0;
        
        // resample. ratio = new frequency / old frequency.
        // return a new & resized signal at new frequency.
        bool resample(std::vector<float> *waves, double ratio, double filterCoefficient);
        bool resample(float *&waves, int32_t wsize, double ratio, double filterCoefficient);

        // lowpass filter. To filter out any frequencies above 4000 Hz on a 44100 Hz
        // recording, filter with coefficient = 44100/8000, as if you were resampling
        // a 8000 Hz recording to 44100 Hz. 
        bool filter(std::vector<float> *waves, double coefficient);
        bool filter(float *&waves, int32_t wsize, double coefficient);

        void impulse(std::vector<float> *waves, float amplitude, double pos, double conv=1);
        void impulse(float *waves, int32_t wsize, float amplitude, double pos, double conv=1);
};

#endif
