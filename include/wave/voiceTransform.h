#ifndef VOICETRANSFORM_H
#define VOICETRANSFORM_H

#include "wave/vocal.h"
#include "wave/vocalTransform.h"

#include <cstdint>
#include <complex>

class VoiceTransform{
    
    protected:

        VocalTransform vot;

        // minFit -> how many times the fundamental should fit in one block
        // minFreq -> minimum fundamental search frequency
        // maxFreq -> maximum fundamental search frequency
        float minFreq, maxFreq, minFit;
        int32_t peaks;

        // audio sampling frequency, window size (in samples)
        int32_t dataFrequency, block, size;
        
        float fundamental;

        float *waves;
        std::complex<float> *spectrum;

    public:
        
        VoiceTransform(
                float minFreq_ = 80,
                float maxFreq_ = 700,
                float minFit_ = 3.5,
                int32_t peaks_ = 7); 

        void set_tolerances(
                float minFreq_, float maxFreq_,
                float minFit_, int32_t peaks_);

        float match_pitch(float frequency);
        float find_pitch();
        void roll_pitch();
        
        void restart_core();
        void roll_core();

        void transform_audio(float *waves_, int32_t size_, int32_t dataFrequency_);

};

#endif
