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
    peakWindowMax(5),
    trustLimit(5),
    minCutoff(0.25f),
    voicedThreshold(0.3f),
    quietThreshold(5e-5f),
    momentumDecay(0.35f),
    rate(frameRate)
{
    if(lower > upper) std::swap(lower, upper);

    min = std::floor(rate / upper);
    max = std::ceil(rate / lower);

    size = 2 * max;
    for(unsigned i=0; i < 2*size; i++) buffer.push_back(0.0f);

    trust = 0;

    momentum.mse.resize(size, 0.0f);
    nonorm.resize(size, 0.0f);
    momentum.top = 0;
}

float Detector::real_period(){
    if(pitch == 0.0f) return 0.0f;
    return rate / pitch;
}

void Detector::feed(const std::vector<float> &data){
    
    for(float i : data) buffer.push_back(i);

    while(buffer.size() > 2 * size) buffer.pop_front();

    // check if the signal is quiet

    {
        float avg = 0.0f, sum = 0.0f;

        for(unsigned i=0; i<max; i++) avg += buffer[size + i];
        avg /= max;

        for(unsigned i=0; i<max; i++) sum += (buffer[size+i] - avg) * (buffer[size+i] - avg);
        sum /= max;

        power = sum;

        quiet = sum < quietThreshold;
    }

    if(quiet){

        voiced = 0;
        confidence = 1;
        trust = 0;
        
        float oldWeight = std::pow(momentumDecay, (float)data.size() / size);

        for(float &i : nonorm) i *= oldWeight;

        return;
    }

    Info one, two;

    // calculate mean square errors and normalize them by dividing
    // by the cumulative average. I think the YIN algorithm does
    // a similar thing. Not sure though.

    // autocorrelation returns ac[j] = sum(i = 0..n, audio[i]*audio[j+i]).
    // while it is in of itself ok for pitch detection, mse seems
    // to work better. We can easily convert autocorrelation to mse.
   
    // if the previous feeds have been voiced, we use cross-correlation between
    // the left and right side of the current point insted of autocorrelation.
    // While autocorrelation performs better on initially picking up the
    // pitch, cross-correlation is more robust during longer voiced segments.

    // The approaches should be roughly equally fast as autocorrelation
    // only requires 1 fft operation per vector insted of 2 but cross-correlation
    // operates on vectors that are half the size.
    
    if(trust > trustLimit){
   
        one.move = data.size() / 2;
        two.move = data.size() - one.move;
        
        // pitch detected. Follow it with correlation.

        std::vector<float> left1(max), right1(max);
        std::vector<float> left2(max), right2(max);

        for(unsigned i=0; i<max; i++){
            left1[i] = buffer[size + i - two.move - max];
            right1[i] = buffer[size + i - two.move];
            left2[i] = buffer[size + i - max];
            right2[i] = buffer[size + i];
        }

        auto process_mse = [&](
                std::vector<float> &left,
                std::vector<float> &right,
                Info &x) -> void
        {

            // calculate correlation

            x.mse = math::correlation(left, right);
            
            // convert to mse

            float sum = 0.0f;
            for(float i : left) sum += i*i;
            for(float i : right) sum += i*i;
            
            for(unsigned i=0; i<max; i++){
                
                if(i+min <= max && sum != 0.0f)
                    x.voiced = std::max(x.voiced, x.mse[i] / sum);
                
                x.mse[i] = (sum - 2 * x.mse[i]) / (max - i);
                sum -= left[i]*left[i] + right[max-i-1]*right[max-i-1];
            }

            x.mse.resize(max+1);
            std::reverse(x.mse.begin(), x.mse.end());

            // normalize

            x.mse[0] = 2.0f;
            sum = 0.0f;
            for(unsigned i=1; i<=max; i++){
                sum += x.mse[i];
                if(sum != 0.0f) x.mse[i] *= i / sum;
            }

            x.mse.resize(size, 2.0f);
        };

        process_mse(left1, right1, one);
        process_mse(left2, right2, two);
    } 
    else {

        // trying to pick up the pitch. initial probing with autocorrelation.
        
        one.move = data.size() / 2;
        two.move = data.size() - one.move;

        std::vector<float> onev(size), twov(size);
        for(unsigned i=0; i<size; i++) onev[i] = buffer[size + i - two.move];
        for(unsigned i=0; i<size; i++) twov[i] = buffer[size + i];

        {
            auto [omse, tmse] = math::autocorrelation(onev, twov);
            one.mse.swap(omse);
            one.mse.resize(size);
            two.mse.swap(tmse);
            two.mse.resize(size);
        }

        auto process_mse = [&](std::vector<float> &time, Info &x) -> void {
           
            // convert to mse

            float sum = 0.0f;
            for(float i : time) sum += i*i;
            sum *= 2;

            x.mse[0] = 2.0f;
            x.voiced = 0.0f;

            for(unsigned i=1; i<size; i++){
                sum -= time[i-1]*time[i-1] + time[size-i]*time[size-i];
                if(i >= min && i <= max && sum != 0.0f) x.voiced = std::max(x.voiced, x.mse[i] / sum);
                x.mse[i] = (sum - 2 * x.mse[i]) / (size - i);
            }

            // normalize

            sum = 0.0f;
            for(unsigned i=1; i<size; i++){
                sum += x.mse[i];
                if(sum != 0.0f) x.mse[i] *= i / sum;
            }
        };
        
        process_mse(onev, one);
        process_mse(twov, two);
    }

    auto normalize = [&](Info &x) -> void {

        float avg = 0.0f;
        for(unsigned i=min; i<max; i++) avg += x.mse[i];
        avg /= (max-min);

        if(avg > 1e-18){
            float iavg = 1.0f / avg;
            for(float &i : x.mse) i *= iavg;
        } else {
            for(float &i : x.mse) i = 2.0f;
        }
    };
    
    auto find_peak = [&](Info &x) -> void {

        unsigned window = std::min(min/2, peakWindowMax);
        
        x.top = 0;

        // keep track of surrounding area using a multiset
        std::multiset<float> s;
        for(unsigned i=0; i<=2*window && i<size; i++) s.insert(x.mse[i]);
        
        x.value = 1.0f;
        for(unsigned i=window; i<=max && i+window+1<size; i++){

            // index i is a minimum in the surrounding range.
            if(x.mse[i] == *s.begin() && i >= min && x.mse[i] < x.value){
                x.value = x.mse[i];
                x.top = i;
                if(x.value < minCutoff) break;
            }

            // update range
            s.erase(s.find(x.mse[i-window]));
            s.insert(x.mse[i+window+1]);
        }
    };

    normalize(one);
    normalize(two);
    
    auto apply_momentum = [&](Info &x){

        float newWeight = x.voiced * power;
        float oldWeight = std::pow(momentumDecay, (float)x.move / size);

        for(unsigned i=0; i<size; i++){
            momentum.mse[i] = newWeight * x.mse[i] + oldWeight * nonorm[i];
        }

        nonorm = momentum.mse;
   
        momentum.voiced = x.voiced;
    };

    apply_momentum(one);
    apply_momentum(two);
    normalize(momentum);

    find_peak(momentum);

    Info &best = momentum;

    // classify.

    if(best.voiced > voicedThreshold){
        
        confidence = 1.0f - best.value;

        voiced = 1;
        period = best.top;

        // quadratic interpolation is useful for high pitches

        pitch = (float)rate / period;
        if(period > 1 && period + 1 <= max){
            
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
        }

        trust++;

    } else {

        confidence = 1.0f - best.voiced;
        voiced = 0;
        trust = 0;

    }
}

void Detector::reset(){
    
    buffer.clear();
    buffer.resize(2*size, 0.0f);
    
    period = 0;
    pitch = 0;
    confidence = 0;
    voiced = 0;
    quiet = 1;
    trust = 0;
    
    momentum.top = 0;
    
    for(float &i : nonorm) i = 0.0f;
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

