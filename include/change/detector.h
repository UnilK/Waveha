#pragma once

#include <vector>
#include <deque>

namespace change {

class Detector {

    // pitch detector. Designed to detect the pitch of a single speaker.

public:
    
    Detector(
            unsigned frameRate = 44100, // input sampling rate, Hz
            float lower = 60,           // pitch search range lower bound, Hz
            float upper = 1000);        // pitch search range upper bound, Hz

    // size fits 2 periods of lower bound frequency. Access only.
    unsigned size;

    // these variables should be used as access-only.
    // they are updated after each feed.
    float period = 0, pitch = 0;
    float confidence = 0;
    bool voiced = 0;
    bool quiet = 1;

    // tweakable variables. Look up the default values in initializer.
    // changing these to some funny values might result in a segfault.
    
    unsigned peakCandidates;
    unsigned peakWindowMin;
    unsigned subRange;
    
    float segmentThreshold;
    float lockThreshold;
    float minCutoff;
    float subCutoff;
    float voicedLimit;
    float nonVoicedLimit;
    float quietThreshold;

    // the pitch is updated only if the buffer has been fed at least <size> samples.
    // time & space complexity of feed is O(size * log(size)).
    void feed(const std::vector<float> &data);
    
    // clear previous information.
    void reset();

private:

    const unsigned rate;
    unsigned min, max;

    std::deque<float> buffer;

};

}

