#include "change/detector.h"
#include "math/fft.h"

#include <cmath>
#include <set>
#include <algorithm>
#include <array>

#include <iostream>

namespace change {

Detector::Detector(unsigned frameRate, float lower, float upper) :
    peakCandidates(10),
    peakWindowMin(5),
    subRange(10),
    segmentThreshold(0.3f),
    lockThreshold(0.7f),
    minCutoff(0.15f),
    subCutoff(0.15f),
    voicedLimit(0.8f),
    nonVoicedLimit(1.2f),
    quietThreshold(1e-4f),
    rate(frameRate)
{

    if(lower > upper) std::swap(lower, upper);

    min = std::floor(rate / upper);
    max = std::ceil(rate / lower);

    size = 2 * max;
}

void Detector::feed(const std::vector<float> &data){
    
    for(float i : data) buffer.push_back(i);

    if(buffer.size() < size) return;

    // prepare observed segments.

    unsigned offset = buffer.size() - size;

    confidence = std::max(0.0f, confidence * (float)(size - data.size()) / size);

    std::array<unsigned, 2> length{size, max};
    if(voiced && confidence > segmentThreshold)
        length[1] = std::min((float)size, 2.5f * period);

    std::vector<float> seg[2];
    for(unsigned j : {0, 1}){
        seg[j].resize(length[j]);
        for(unsigned i=0; i<length[j]; i++) seg[j][i] = buffer[offset + i];
    }

    // calculate mean square errors and normalize them by dividing
    // by the cumulative average. I think the YIN algorithm does
    // a similar thing. Not sure though.

    // autocorrelation returns ac[j] = sum(audio[i]*audio[j+i]).
    // while it is in of itself ok for pitch detection, mse seems
    // to work better. We can easily convert autocorrelation to mse.
    auto mse = math::autocorrelation(seg[0], seg[1]);
    float mseAvg[2] = {0.0f, 0.0f};
    
    for(unsigned j : {0, 1}){

        // convert to mse

        unsigned n = seg[j].size();

        float sum = 0.0f;
        for(float i : seg[j]) sum += i*i;
        sum *= 2;

        mse[j][0] = nonVoicedLimit;
        for(unsigned i=1; i<n; i++){
            sum -= seg[j][i-1]*seg[j][i-1] + seg[j][n-i]*seg[j][n-i];
            mse[j][i] = (sum - 2 * mse[j][i]) / (n - i);
        }

        // normalize

        sum = 0.0f;
        for(unsigned i=1; i<n; i++){
            sum += mse[j][i];
            if(sum != 0.0f) mse[j][i] /= (sum/i);
        }
    
        // this is used for catching octave errors later on
        for(unsigned i=0; i<length[j]/2; i++) mseAvg[j] += mse[j][i];
        mseAvg[j] /= length[j]/2 - min;
    }
    
    // find minimum peaks

    unsigned window = std::max(min/2, peakWindowMin);

    std::vector<unsigned> mins[2];
    unsigned top[2] = {0, 0};

    for(unsigned j : {0, 1}){

        // keep track of surrounding area using a multiset
        std::multiset<float> s;
        for(unsigned i=0; i<=2*window && i < mse[j].size(); i++) s.insert(mse[j][i]);
        
        for(unsigned i=window; i<=length[j]/2 && i+window+1 < mse[j].size(); i++){

            // index i is a minimum in the surrounding range.
            if(mse[j][i] == *s.begin() && i >= min) mins[j].push_back(i);

            // update range
            s.erase(s.find(mse[j][i-window]));
            s.insert(mse[j][i+window+1]);
        }
        
        // if we find a minimum below a certain threshold, we
        // choose it over the later minimums that might be smaller.
        // this greatly helps in cases where many periods fit into the segment.

        for(unsigned i : mins[j]){
            if(mse[j][i] <= minCutoff){
                top[j] = i;
                break;
            }
        }

        std::sort(mins[j].begin(), mins[j].end(),
                [&](auto l, auto r){ return mse[j][l] < mse[j][r]; });

        mins[j].resize(peakCandidates, 0);


        if(top[j] == 0) top[j] = mins[j][0];
    }

    // pick best peak candidate

    float mseAtBest = nonVoicedLimit;
    unsigned best = 0;

    auto get_best = [&](int j, unsigned optimal, float optMse) -> unsigned {

        unsigned original = optimal;

        // check if we caught a subfrequency.
        for(unsigned div=2; original / div >= min; div++){
            unsigned fx =  original / div;
            for(unsigned i : mins[j]){
                unsigned d = fx > i ? fx - i : i - fx;
                if(d < subRange && i != 0 && optMse * (1.0f - subCutoff) < mseAvg[j] - mse[j][i]){
                    optimal = std::min(optimal, i);
                }
            }
        }
        
        mseAtBest = mse[j][optimal];

        return optimal;
    };

    if(voiced && confidence > lockThreshold){

        // this makes it easier to capture the pitch at the
        // very end of the voiced part.

        best = get_best(1, top[1], mseAvg[1] - mse[1][top[1]]);
    } else {

        best = get_best(0, top[0], mseAvg[0] - mse[0][top[0]]);
        if(2 * best < length[1]) get_best(1, top[1], mseAvg[1] - mse[1][top[1]]);

    }

    // check if signal is quiet

    {

        unsigned len = best;
        if(best == 0) len = max;

        float sum = 0.0f;
        for(unsigned i=0; i<len; i++) sum += buffer[offset + i] * buffer[offset + i];
        sum /= len;

        if(sum < quietThreshold) quiet = 1;
        else quiet = 0;
    }

    // classify

    if(quiet){
        
        voiced = 0;
        confidence = 1;
        pitch = 0.0f;
        period = 0.0f;

    }
    else if(mseAtBest <= voicedLimit){
        
        voiced = 1;
        confidence = (voicedLimit - mseAtBest) / voicedLimit;

        period = best;
        if(best != 0) pitch = rate / period;
        else pitch = 0.0f;

    } else {

        voiced = 0;
        confidence = std::min(1.0f, (mseAtBest - voicedLimit) / (nonVoicedLimit - voicedLimit));

        period = best;
        if(best != 0) pitch = rate / period;
        else pitch = 0.0f;

    }

    while(buffer.size() > size) buffer.pop_front();

}

void Detector::reset(){
    
    buffer.clear();
    
    period = 0;
    pitch = 0;
    confidence = 0;
    voiced = 0;
    quiet = 1;
}

}

