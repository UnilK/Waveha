#pragma once

#include <vector>
#include <mutex>


namespace wave {

class AudioBuffer{

public:

    AudioBuffer(uint32_t frameRate = 44100, uint32_t channels = 1);
    
    // get data and move internal pointer
    std::vector<float> get_move(uint32_t amount);

    // get data but don't move the pointer
    std::vector<float> get_stationary(uint32_t amount);

    // add data (+=) to the audio buffer and move the write position by writeOffset
    // default writeOffset is the amount of samples written.
    void add_data(float *waves, uint32_t amount, int32_t writeOffset = -1);
    void add_data(std::vector<float> &waves, int32_t writeOffset = -1);

    // are enough samples loaded?
    bool satisfied();

    // the write pointer's distance from end of buffer
    uint32_t get_write_offset();
    
    // read pointer distance from end of file
    uint32_t get_read_max();

    // get number of samples needed to satisfy the buffer
    uint32_t get_hunger();

    // how many audio samples should be preloaded?
    uint32_t preloadSize = 1<<15;
    
protected:

    uint32_t frameRate, channels;

    // clean old audio. Amortized O(n).
    void clean_audio();
    
    // clear all audio and reset pointers.
    void reset();
    
    std::vector<float> audio;
    std::mutex audioLock;

    uint32_t readPointer = 0;
    uint32_t writePointer = 0;

};

}
