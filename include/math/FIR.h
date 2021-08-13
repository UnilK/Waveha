#ifndef FIR_H
#define FIR_H

#include <cstdint>
#include <vector>

namespace math{

class FIR{
    
    // template class for Finite Impulse Responses.

protected:

    // the "quality" of the impulse i.e. how long the impulse is simulated,
    // to each side.
    int32_t length = 0;

public:

    FIR();
    FIR(int32_t length_);

    // evaluate impulse function. (pure virtual - override this!) 
    virtual float get(double num) = 0;
    
    // resample. ratio = new frequency / old frequency.
    // return a new & resized signal at new frequency.
    bool resample(std::vector<float> &waves, double ratio, double filterCoefficient);

    // lowpass filter. To filter out any frequencies above 4000 Hz on a 44100 Hz
    // recording, filter with coefficient = 44100/8000, as if you were resampling
    // a 8000 Hz recording to 44100 Hz. 
    bool filter(std::vector<float> &waves, double coefficient);

    bool impulse(std::vector<float> &waves, float amplitude, double pos, double conv=1);

};

}

#endif
