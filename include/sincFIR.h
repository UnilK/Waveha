#ifndef SINCFIR_H
#define SINCFIR_H

#include "FIR.h"

class FastSincFIR : public FIR{
    
    protected:

        /*
           sinc(t) = sin(pi*t)/(pi*t)
        
           caution, the get() doesn't have any segmentation fault safeguards
           for efficiency reasons.
        
           might be a bit embarrassing if this turns out to be slower
           than the precise implementation...
        */

        int32_t resolution = 0, size = 0;
        float *sinc;

    public:

        FastSincFIR();
        FastSincFIR(int32_t length_, int32_t resolution_);

        bool initialize(int32_t length_, int32_t resolution_);
        float get(double num);
};

class PreciseSincFIR : public FIR{
    
    public:

        PreciseSincFIR();
        PreciseSincFIR(int32_t length_);

        float get(double num);
};

#endif
