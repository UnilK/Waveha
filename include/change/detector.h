#pragma once

#include <vector>
#include <deque>

namespace change {

class Detector {

    // pitch detector. Designed to detect the pitch of a single source
    // in good conditions.

public:
    
    Detector(
            unsigned frameRate = 44100, // input sampling rate, Hz
            float lower = 60,           // pitch search range lower bound, Hz
            float upper = 900);         // pitch search range upper bound, Hz

    // size fits 2 periods of lower bound frequency. Access only.
    unsigned size;

    // the variables below should be used as access-only.
    // they are updated after each feed.

    // note that it doesn't neccesarily hold true that rate / pitch = period
    // since the pitch is interpolated and the period is not.
    // real_period returns the real valued period i.e. rate / pitch.

    unsigned period = 0;
    float pitch = 0;
    float confidence = 0;
    bool voiced = 0;
    bool quiet = 1;

    float real_period();

    // tweakable variables. Look up the default values in the initializer.
    // changing these to some funny values might result in segfaults.
    
    unsigned peakWindowMax;
    unsigned trustLimit;

    float minCutoff;
    float voicedThreshold;
    float quietThreshold;
    float momentumDecay;

    // the pitch is updated only if the buffer has been fed at least <size> samples.
    // time complexity of feed is O(size * log(size)).
    void feed(const std::vector<float> &data);
    
    // clear previous information.
    void reset();

    // get one period from the buffer. The buffer has a lag of size samples.
    std::vector<float> get(unsigned amount = 0);
    
    // get two periods from the buffer. One extending after the lag and one before.
    std::vector<float> get2(unsigned amount = 0);

    // get the momentum mse graph. for debugging purposes.
    std::vector<float> get_mse();

private:

    const unsigned rate;
    unsigned min, max, trust;
    float power;

    std::deque<float> buffer;

    struct Info {
        unsigned move, top;
        std::vector<float> mse;
        float voiced, value;
    };

    Info momentum;
    std::vector<float> nonorm;

};

}

