#include "change/detector.h"
#include "math/fft.h"
#include "math/ft.h"

#include <cmath>
#include <set>
#include <algorithm>
#include <array>
#include <cassert>

#include <iostream>

namespace change {

Detector::Detector(unsigned frameRate, float lower, float upper) :
    peakCandidates(10),
    peakWindowMax(5),
    subRange(10),
    minCutoff(0.15f),
    subCutoff(0.92f),
    voicedLimit(0.4f),
    nonVoicedLimit(0.9f),
    quietThreshold(5e-5f),
    movementLimit(0.1f),
    decay(0.9),
    rate(frameRate)
{
    if(lower > upper) std::swap(lower, upper);

    min = std::floor(rate / upper);
    max = std::ceil(rate / lower);

    size = 2 * max;
    for(unsigned i=0; i < 2*size; i++) buffer.push_back(0.0f);

    momentum.size = size;
    momentum.half = max;
    momentum.mse.resize(size, 0.0f);
    momentum.avg = 0.0f;
    momentum.min = 0.0f;
}

float Detector::real_period(){
    if(pitch == 0.0f) return 0.0f;
    return rate / pitch;
}

void Detector::feed(const std::vector<float> &data){
    
    for(float i : data) buffer.push_back(i);

    while(buffer.size() > 2 * size) buffer.pop_front();

    // prepare observed segments.

    Info one, two;
    one.size = size;
    two.size = std::min((float)size, 2.5f * period);
    one.time.resize(one.size);
    two.time.resize(two.size);
    one.half = one.size / 2;
    two.half = two.size / 2;

    for(unsigned i=0; i<one.size; i++) one.time[i] = buffer[size + i];
    for(unsigned i=0; i<two.size; i++) two.time[i] = buffer[size + i];

    // calculate mean square errors and normalize them by dividing
    // by the cumulative average. I think the YIN algorithm does
    // a similar thing. Not sure though.

    // autocorrelation returns ac[j] = sum(i = 0..n, audio[i]*audio[j+i]).
    // while it is in of itself ok for pitch detection, mse seems
    // to work better. We can easily convert autocorrelation to mse.
   
    {
        auto [omse, tmse] = math::autocorrelation(one.time, two.time);
        one.mse.swap(omse);
        two.mse.swap(tmse);
        one.mse.resize(one.size);
        two.mse.resize(two.size);
    }

    auto process_mse = [&](Info &x) -> void {
       
        // convert to mse

        float sum = 0.0f;
        for(float i : x.time) sum += i*i;
        x.power = sum / x.size;
        sum *= 2;

        x.mse[0] = nonVoicedLimit;
        for(unsigned i=1; i<x.size; i++){
            sum -= x.time[i-1]*x.time[i-1] + x.time[x.size-i]*x.time[x.size-i];
            x.mse[i] = (sum - 2 * x.mse[i]) / (x.size - i);
        }

        // normalize

        sum = 0.0f;
        for(unsigned i=1; i<x.size; i++){
            sum += x.mse[i];
            float d = sum / i;
            if(d != 0.0f) x.mse[i] /= d;
        }
    
    };

    auto normalize = [&](Info &x) -> void {

        x.avg = 0.0f;
        for(unsigned i=min; i<x.half; i++) x.avg += x.mse[i];
        x.avg /= (x.half-min);

        if(x.avg > 1e-9){
            float iavg = 1.0f / x.avg;
            for(float &i : x.mse) i *= iavg;
        } else {
            for(float &i : x.mse) i = nonVoicedLimit;
        }

        x.min = 1e9;
        for(unsigned i=min; i<x.half; i++) x.min = std::min(x.min, x.mse[i]);

    };

    process_mse(one);
    normalize(one);
    
    // apply momentum

    std::vector<float> nonorm;

    {

        float newWeight = one.power * (1.0f - one.min);
        float oldWeight = decay * std::max(0.0f, ((float)size - data.size()) / (float)size);

        for(unsigned i=0; i<size; i++){
            momentum.mse[i] = newWeight * one.mse[i] + oldWeight * momentum.mse[i];
        }

        nonorm = momentum.mse;
        normalize(momentum);
    }

    // find minimum peaks

    auto process_peaks = [&](Info &x) -> void {

        unsigned window = std::min(min/2, peakWindowMax);
        
        x.peaks.clear();
        x.top = 0;

        // keep track of surrounding area using a multiset
        std::multiset<float> s;
        for(unsigned i=0; i<=2*window && i<x.size; i++) s.insert(x.mse[i]);
        
        for(unsigned i=window; i<=x.half && i+window+1<x.size; i++){

            // index i is a minimum in the surrounding range.
            if(x.mse[i] == *s.begin() && i >= min) x.peaks.push_back(i);

            // update range
            s.erase(s.find(x.mse[i-window]));
            s.insert(x.mse[i+window+1]);
        }
        
        // if we find a minimum below a certain threshold, we
        // choose it over the later minimums that might be smaller.
        // this greatly helps in cases where many periods fit into the segment.

        for(unsigned i : x.peaks){
            if(x.mse[i] <= minCutoff){
                x.top = i;
                break;
            }
        }

        std::sort(x.peaks.begin(), x.peaks.end(),
                [&](auto l, auto r){ return x.mse[l] < x.mse[r]; });

        x.peaks.resize(peakCandidates, 0);

        if(x.top == 0) x.top = x.peaks[0];
        x.value = x.mse[x.top];
    };

    process_peaks(momentum);

    // check if the signal is quiet

    bool quiet = 0;
    {
        unsigned len = max;

        float avg = 0.0f, sum = 0.0f;

        for(unsigned i=0; i<len; i++) avg += buffer[size + i];
        avg /= len;

        for(unsigned i=0; i<len; i++) sum += (buffer[size+i] - avg) * (buffer[size+i] - avg);
        sum /= len;

        quiet = sum < quietThreshold;
    }

    // check if we caught a multiple.
    
    auto detect_multiple = [&](Info &x) -> void {

        unsigned original = x.top;
        unsigned optimal = x.top;

        for(unsigned div=2; original / div >= min; div++){
            unsigned fx =  original / div;
            for(unsigned i : x.peaks){
                unsigned d = fx > i ? fx - i : i - fx;
                if(d < subRange && i != 0 && 1 - x.mse[i] > (1 - x.value) * subCutoff){
                    optimal = std::min(optimal, i);
                }
            }
        }
        
        x.top = optimal;
        x.value = x.mse[optimal];
    };

    detect_multiple(momentum);

    Info &best = momentum;

    // classify

    if(quiet){
        
        voiced = noise = 0;
        confidence = 1;
        pitch = 0.0f;
        period = 0;

    }
    else if(one.min <= voicedLimit){
        
        voiced = 1;
        noise = 0;
        confidence = (voicedLimit - best.value) / voicedLimit;

        period = best.top;

        // quadratic interpolation is useful for high pitches

        if(period > 1 && period + 1 < best.half){
            
            std::array<float, 3> val = {
                best.mse[period-1],
                best.mse[period],
                best.mse[period+1]
            };

            // construct a parabola using the three points around
            // the peak. set the peak as origo and find the
            // bottom of the parabola. val[0] and val[2] are at distance
            // 1 from val[1] on the x -axis.

            // y = ax^2 + bx

            float y0 = val[0] - val[1], y1 = val[2] - val[1];
            float a = (y1 + y0) / 2;
            float b = (y1 - y0) / 2;

            float bottom = -b / (2*a);

            if(bottom > -1.0f && bottom < 1.0f) pitch = (float)rate / (period + bottom);
            else pitch = (float)rate / period;

        }
        else {
            pitch = (float)rate / period;
        }

    } else {

        voiced = 0;
        noise = 1;
        confidence = std::min(1.0f, (best.value - voicedLimit) / (nonVoicedLimit - voicedLimit));

        period = best.top;
        if(period != 0) pitch = rate / period;
        else pitch = 0.0f;

    }

    momentum.mse.swap(nonorm);
}

void Detector::reset(){
    
    buffer.clear();
    for(unsigned i=0; i < 2*size; i++) buffer.push_back(0.0f);
    
    period = 0;
    pitch = 0;
    confidence = 0;
    voiced = 0;
    quiet = 1;
    noise = 0;
    
    for(float &i : momentum.mse) i = 0.0f;
    momentum.avg = 0.0f;
    momentum.min = 0.0f;
}

std::vector<float> Detector::get(){

    std::vector<float> samples(period);
    for(unsigned i=0; i<period; i++) samples[i] = buffer[size + i];

    return samples;
}

std::vector<float> Detector::get2(){

    std::vector<float> samples(2*period);
    for(unsigned i=0; i<2*period; i++) samples[i] = buffer[size - period/2 + i];

    return samples;
}

std::vector<float> Detector::get_mse(){
    return momentum.mse;
}

}

/*
void Detector::feed(const std::vector<float> &data){
    
    for(float i : data) buffer.push_back(i);

    while(buffer.size() > 2 * size) buffer.pop_front();

    // prepare observed segments.

    confidence = confidence * std::max(0.0f, std::sqrt((float)(size - data.size()) / size));

    std::array<unsigned, 2> length{size, max};
    if(notrust <= distrust){
        length[1] = std::min((float)size, 2.5f * previousTrusted);
        std::swap(length[1], length[0]);
    }
    else if(voiced && confidence > lowTrust) length[1] = std::min((float)size, 2.5f * period);

    std::vector<float> seg[2];
    for(unsigned j : {0, 1}){
        seg[j].resize(length[j]);
        for(unsigned i=0; i<length[j]; i++) seg[j][i] = buffer[size + i];
    }

    // calculate mean square errors and normalize them by dividing
    // by the cumulative average. I think the YIN algorithm does
    // a similar thing. Not sure though.

    // autocorrelation returns ac[j] = sum(i = 0..n, audio[i]*audio[j+i]).
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

    unsigned window = std::min(min / 2, peakWindowMin);

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
    unsigned best = 0, bestIndex = 0;

    auto get_best = [&](int j, unsigned optimal, float optMse) -> unsigned {

        unsigned original = optimal;

        // check if we caught a multiple.
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
        bestIndex = j;

        return optimal;
    };

    if(trust >= distrust){

        // if we trust the previous pitch assesment to be accurate
        // we can just follow it around.

        unsigned j = length[0] > length[1];

        best = top[j];
        unsigned minDist = std::abs((int)top[j] - (int)period);
        

        for(unsigned i : mins[j]){
            unsigned dist = std::abs((int)i - (int)period);
            if(dist < minDist){
                minDist = dist;
                best = i;
            }
        }

        mseAtBest = mse[j][best];
        bestIndex = j;

    } else {

        best = get_best(0, top[0], mseAvg[0] - mse[0][top[0]]);
        if(2 * best < length[1]
                && std::abs((int)top[0]-(int)top[1]) < (int)subRange
                && length[0] > length[1]){
            best = get_best(1, top[1], mseAvg[1] - mse[1][top[1]]);
        }
    }

    { // check if signal is quiet or noise

        unsigned len = best;
        if(len < min) len = max;
        if(len < 2 * min && 2 * len < max) len *= 2;

        float avg = 0.0f, sum = 0.0f;

        for(unsigned i=0; i<len; i++) avg += buffer[size + i];
        avg /= len;

        for(unsigned i=0; i<len; i++) sum += (buffer[size+i] - avg) * (buffer[size+i] - avg);
        sum /= len;

        quiet = sum < quietThreshold;

        // check noise by comparing low pass filtered energy to total energy

        std::vector<float> lowpass(max);
        for(unsigned i=0; i<max; i++) lowpass[i] = buffer[size+i];

        lowpass = math::ift(math::ft(lowpass, lowPass), len);

        float lowsum = 0.0f;
        for(unsigned i=0; i<len; i++) lowsum += lowpass[i] * lowpass[i];
        lowsum /= len;

        // blocks classified as noise cannot be highly trusted.
        noise = lowsum / sum < noiseThreshold;
    }

    // classify

    if(quiet){
        
        voiced = 0;
        confidence = 1;
        pitch = 0.0f;
        period = 0.0f;
        trust = 0;
        notrust = std::min(notrust, distrust) + 1;

    }
    else if(mseAtBest <= voicedLimit){
        
        voiced = 1;
        confidence = (voicedLimit - mseAtBest) / voicedLimit;

        float previousPitch = pitch;

        period = best;

        // quadratic interpolation is useful for high pitches

        if(best > 1 && best + 1 < length[bestIndex]){
            
            std::array<float, 3> val = {
                mse[bestIndex][best-1],
                mse[bestIndex][best],
                mse[bestIndex][best+1]
            };

            {
                // construct a parabola using the three points around
                // the peak. set the peak as origo and find the
                // bottom of the parabola. val[0] and val[2] are at distance
                // 1 from val[1] on the x -axis.

                // y = ax^2 + bx

                float y0 = val[0] - val[1], y1 = val[2] - val[1];
                float a = (y1 + y0) / 2;
                float b = (y1 - y0) / 2;

                float bottom = -b / (2*a);

                if(bottom > -1.0f && bottom < 1.0f)
                    pitch = (float)rate / (period + bottom);
                else
                    pitch = (float)rate / period;
            }

        } else {
            pitch = (float)rate / period;
        }

        if(best == 0) pitch = 0.0f;

        if(std::max(pitch / previousPitch, previousPitch / pitch) > movementLimit){
            
            trust = 0;
            notrust = std::min(notrust, distrust) + 1;
        }
        else if((confidence > highTrust || trust > 2 * distrust) && (!noise || confidence > 0.7f)){
            trust = std::min(5 * distrust, trust + 1);
            previousTrusted = period;
            notrust = 0;
        }
        else if(confidence > lowTrust){
            trust = trust < 1 ? 0 : trust - 1;
            notrust = std::min(notrust, distrust) + 1;
        }
        else {
            trust = trust < 3 ? 0 : trust - 3;
            notrust = std::min(notrust, distrust) + 1;
        }

    } else {

        voiced = 0;
        confidence = std::min(1.0f, (mseAtBest - voicedLimit) / (nonVoicedLimit - voicedLimit));

        period = best;
        if(best != 0) pitch = rate / period;
        else pitch = 0.0f;

        trust = 0;
        notrust = std::min(notrust, distrust) + 1;
    }

}

 */
