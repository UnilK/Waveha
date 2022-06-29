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
            float upper = 600);         // pitch search range upper bound, Hz

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
    bool noise = 0;

    float real_period();

    // tweakable variables. Look up the default values in initializer.
    // changing these to some funny values might result in segfaults.
    
    unsigned peakCandidates;
    unsigned peakWindowMin;
    unsigned subRange;
    unsigned distrust;
    unsigned lowPass;

    float lowTrust;
    float highTrust;
    float minCutoff;
    float subCutoff;
    float voicedLimit;
    float nonVoicedLimit;
    float quietThreshold;
    float noiseThreshold;
    float movementLimit;

    // the pitch is updated only if the buffer has been fed at least <size> samples.
    // time & space complexity of feed is O(size * log(size)).
    void feed(const std::vector<float> &data);
    
    // clear previous information.
    void reset();

    // get one period from the buffer. The buffer has a lag of size samples.
    std::vector<float> get();
    
    // get two periods from the buffer. Lag is size + period / 2.
    std::vector<float> get2();

private:

    const unsigned rate;
    unsigned min, max, trust;
    unsigned previousTrusted, notrust;

    std::deque<float> buffer;

};

}

