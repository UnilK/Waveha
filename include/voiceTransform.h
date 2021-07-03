#ifndef VOICETRANSFORM_H
#define VOICETRANSFORM_H

#include <vector>
#include <complex>

#include "vocal.h"
#include "vocalTransform.h"
#include "FFT.h"
#include "sinusoidFT.h"

class VoiceTransform{
    
    protected:

        FFT fft;
        SinusoidFT sft;

        VocalTransform vot;

        // audio data sampling frequency, window size (in samples)
        int32_t dataFrequency = 44100, N = 1<<10;

        float minFreq = 80, maxFreq = 750;

    public:
        
        VoiceTransform(); 
        VoiceTransform(int32_t dataFrequency_, int32_t N_);

        void set_frequency(float dataFrequency_);
        void set_size(int32_t N_);

        void set_tolerances(float min, float max);

        float get_pitch_match(std::complex<float> *spectrum, float frequency, float offSet = 0);

        float get_pitch(float *waves);
        
        void transform_audio(std::vector<float> waves);
        void transform_audio(float *waves, int32_t size);

};

#endif
