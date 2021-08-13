#ifndef WAVETRANSFORM_H
#define WAVETRANSFORM_H

#include "wave/vocal.h"
#include "wave/vocalTransform.h"

#include <cstdint>
#include <complex>
#include <vector>

namespace wave{

class WaveTransform{

protected:

    VocalTransform vot;

    // minFit -> how many times the fundamental should fit in one block
    // minFreq -> minimum fundamental search frequency
    // maxFreq -> maximum fundamental search frequency
    // freqs -> number of frequencies tracked (fundamental + subfrequencies)
    float minFreq, maxFreq, minFit;
    int32_t peaks, freqs;

    std::vector<float> source, processed;
    std::complex<float> previousPhase;
    
    // audio sampling frequency.
    int32_t dataFrequency;
   
    // audio read and write position, previous read position.
    int32_t rpos, wpos;

    // window size for fundamental search & audio classification.
    // previous block begin position.
    int32_t block, pblock;

public:
    
    WaveTransform(
            int32_t dataFrequency_ = 44100,
            float minFreq_ = 80,
            float maxFreq_ = 600,
            float minFit_ = 3,
            int32_t peaks_ = 7,
            int32_t freqs_ = 50); 

    ~WaveTransform();

    bool set_dataFrequency(int32_t dataFrequency_);
    bool set_tolerances(
            float minFreq_, float maxFreq_, float minFit_,
            int32_t peaks_, int32_t freqs_);

    float match_pitch(float frequency);
    float find_pitch();
    bool roll_pitch();
    
    bool init_core();
    bool roll_core();

    // link source audio to be processed. The data is copied.
    bool link_audio(std::vector<float> &audio);
    bool link_audio(float *audio, int32_t size);
    
    // add audio at the end of the previously linked audio.
    bool add_audio(std::vector<float> &audio);
    bool add_audio(float *audio, int32_t size);

    // access audio
    std::vector<float> get_processed();
    std::vector<float> get_source();
    float *processed_pointer();
    float *source_pointer();
    int32_t processed_size();
    int32_t source_size();
    int32_t write_position();
    int32_t read_position();

    // get the last {size} fully processed samples. default size is th internal block size.
    std::vector<float> last_processed(int32_t size=-1);
    std::vector<float> last_source(int32_t size=-1);

    // gentle clean, leave some of the already fully used data behind.
    bool clean_processed();
    bool clean_source();

    // hard clear, remove all fully or partially used data
    bool clear_processed();
    bool clear_source();
};

}

#endif
