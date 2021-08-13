#include "wave/pitchHandler.h"
#include "math/FFT.h"

#include <math.h>

namespace wave{

float match_pitch(
        std::vector<std::complex<float> > &spectrum,
        int32_t dataFrequency,
        float frequency,
        int32_t peaks){
    return match_pitch(
            spectrum.data(),
            spectrum.size(),
            dataFrequency,
            frequency,
            peaks);
}

float match_pitch(
        std::complex<float> *spectrum,
        int32_t size,
        int32_t dataFrequency,
        float frequency,
        int32_t peaks){

    /*
        assume that the spectrum is from a voice that has
        a fundamental frequency of {frequency}. Calculate
        something that correlates to the sum of amplitudes
        of the fundamental & its subfrequencies.
    */

    float sum = 0;
    frequency = std::abs(frequency*size/dataFrequency);

    for(int32_t i=1; frequency*i<(float)size/2 && i<=peaks; i++){
        int32_t pos = (int32_t)std::floor(frequency*i);
        float d = frequency*i-pos;
        sum += std::abs(spectrum[pos]*(1.0f-d)-spectrum[pos+1]*d);
    }

    return sum;
}

float find_pitch(
        std::vector<float> &waves,
        int32_t dataFrequency,
        float minFreq,
        float maxFreq,
        int32_t peaks){
    return find_pitch(
            waves.data(),
            waves.size(),
            dataFrequency,
            minFreq,
            maxFreq,
            peaks);
}

float find_pitch(
        float *waves,
        int32_t size,
        int32_t dataFrequency,
        float minFreq,
        float maxFreq,
        int32_t peaks){
   
    std::vector<float> graph = pitch_match_graph(
            waves, size, dataFrequency, minFreq, maxFreq, peaks);

    float maxAmp = 0, freq = minFreq, step = (maxFreq-minFreq)/size;
    int32_t pos = 0;
    for(float i=minFreq; i<=maxFreq; i+=step){
        if(graph[pos] > maxAmp){
            freq = i;
            maxAmp = graph[pos];
        }
        pos++;
    }

    return freq;
}

int32_t best_pitch_length(
        std::vector<float> &waves,
        int32_t length_approx,
        float tolerance){
    return best_pitch_length(
            waves.data(),
            waves.size(),
            length_approx,
            tolerance);
}

int32_t best_pitch_length(
        float *waves,
        int32_t size,
        int32_t length_approx,
        float tolerance){
    
    int32_t len = 1;
    while(len < length_approx) len *= 2;
    len *= 2;

    std::vector<float> graph = wave_correlation_graph(
            waves, size, length_approx, tolerance);

    float maxAmp = 0;
    int32_t best = length_approx;
    int32_t min = (int32_t)std::ceil(length_approx/tolerance);
    int32_t max = std::min(2*len, (int32_t)std::ceil(length_approx*tolerance)+1);

    for(int32_t length=min; length<max; length++){
        if(graph[length] > maxAmp){
            maxAmp = graph[length];
            best = length;
        }
    }

    return best;
}

std::vector<float> pitch_match_graph(
        std::vector<float> &waves,
        int32_t dataFrequency,
        float minFreq,
        float maxFreq,
        int32_t peaks){
    return pitch_match_graph(
            waves.data(),
            waves.size(),
            dataFrequency,
            minFreq,
            maxFreq,
            peaks);
}

std::vector<float> pitch_match_graph(
        float *waves,
        int32_t size,
        int32_t dataFrequency,
        float minFreq,
        float maxFreq,
        int32_t peaks){

    std::vector<float> graph(size);
    std::vector<std::complex<float> > spectrum = math::fft(waves, size);
    
    // identify the fundamental by testing pitch matches.
    float step = (maxFreq-minFreq)/size;
    int32_t pos = 0;
    for(float i=minFreq; i<=maxFreq; i+=step){
        graph[pos++] = match_pitch(spectrum, dataFrequency, i, peaks);
    }

    return graph; 
}

std::vector<float> wave_correlation_graph(
        std::vector<float> &waves,
        int32_t length_approx,
        float tolerance){
    return wave_correlation_graph(
            waves.data(),
            waves.size(),
            length_approx,
            tolerance); 
}

std::vector<float> wave_correlation_graph(
        float *waves,
        int32_t size,
        int32_t length_approx,
        float tolerance){

    int32_t len = 1;
    while(len < length_approx) len *= 2;
    len *= 2;

    std::vector<float> graph(2*len);
    std::vector<std::complex<float> > first(len, 0.0f), second(2*len, 0.0f);

    for(int32_t i=0; i<std::min(size, length_approx); i++){
        first[i] = waves[i];
    }
    for(int32_t i=0; length_approx+i<std::min(size, 2*length_approx); i++){
        second[length_approx-i] = waves[length_approx+i];
    }

    math::in_place_fft(first, 0);
    math::in_place_fft(second, 0);
    for(int32_t i=0; i<len; i++) first[i] *= second[i];
    math::in_place_fft(first, 1);
    
    for(int32_t i=0; i<2*len; i++) graph[i] = first[i].real();

    return graph;
} 

}

