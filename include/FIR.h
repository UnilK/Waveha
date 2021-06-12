#ifndef FIR_H
#define FIR_H

#include <cstdint>
#include <vector>

class FIR{
    
    // template class for Finite Impulse Responses.

    public:

        FIR();

        // evaluate function 
        virtual float get(double num) = 0;
        
        // resample. ratio = new
        virtual bool resample(std::vector<float> *waves,
                double ratio, double filterCoefficient = 1) = 0;
        virtual bool resample(float *&waves, int32_t wsize,
                double ratio, double filterCoefficient = 1) = 0;
       
        // lowpass filter
        virtual bool filter(std::vector<float> *waves, double coefficient) = 0;
        virtual bool filter(float *&waves, int32_t wsize, double coefficient) = 0;

};

class SincFIR : public FIR{
    
    protected:

        // sinc(t) = sin(pi*t)/(pi*t)

        // zeros zero-crossings, each interval between zero-crossings
        // is evaluated at resolution points and stored to sinc.

        int32_t zeros = 0, resolution = 0, size = 0;
        float *sinc;

    public:

        SincFIR();
        SincFIR(int32_t zeros_, int32_t resolution_);

        // zeros*resolution < 2^31
        bool initialize(int32_t zeros_, int32_t resolution_);
        
        float get(double num);
        
        bool resample(std::vector<float> *waves,
                double ratio, double filterCoefficient = 1);
        bool resample(float *&waves, int32_t wsize,
                double ratio, double filterCoefficient = 1);
       
        bool filter(std::vector<float> *waves, double coefficient);
        bool filter(float *&waves, int32_t wsize, double coefficient);

};

#endif
