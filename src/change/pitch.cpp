#include "change/pitch.h"
#include "math/fft.h"
#include "math/ft.h"
#include "math/constants.h"
#include "ml/stack.h"

#include <math.h>
#include <algorithm>

namespace change {

PeakMatchVars defaultPeakVars;
CorrelationVars defaultCorrelationVars;

float sign(float x){ return (int)(x > 0) - (x < 0); }

std::vector<float> peak_match_graph(const std::vector<float> &audio, PeakMatchVars vars){
    
    auto spectrum = math::fft(audio);

    std::vector<float> plot(audio.size(), 0.0f);

    for(unsigned i = 2 * vars.peaks; i * 2 < plot.size(); i++){
        
        float fundamental = (float)plot.size() / i;

        for(int j = 1; j <= vars.peaks; j++){
            
            float frequency = fundamental * j;
            int k = std::floor(frequency);
            float d  = frequency - k;

            if(k+1 > (int)audio.size() / 2) break;

            // there's a phase flip on the peak so use - instead of +.
            plot[i] += std::pow(std::abs(spectrum[k] * (1 - d) - spectrum[k+1] * d), vars.exponent);

        }

    }

    return plot;

}

std::vector<float> correlation_graph(const std::vector<float> &audio, CorrelationVars vars){
    
    auto copy = audio;

    if(vars.sign) for(float &i : copy) i = sign(i) * std::pow(std::abs(i), vars.exponent);
    else for(float &i : copy) i = std::pow(std::abs(i), vars.exponent);

    auto reverseCopy = copy;
    std::reverse(reverseCopy.begin(), reverseCopy.end());

    auto plot = math::convolution(copy, reverseCopy);

    return plot;
}

unsigned pitch(const std::vector<float> &audio, CorrelationVars vars){
    
    auto copy = audio;

    if(vars.sign) for(float &i : copy) i = sign(i) * std::pow(std::abs(i), vars.exponent);
    else for(float &i : copy) i = std::pow(std::abs(i), vars.exponent);

    auto reverseCopy = copy;
    std::reverse(reverseCopy.begin() + 1, reverseCopy.end());

    auto plot = math::convolution(copy, reverseCopy);

    unsigned max = 50;
    float m = 0.0f;
    unsigned n = audio.size()-1;
    
    for(unsigned i=50; n+i<plot.size() && i < 800; i++){
        if(m < plot[n+i]){
            m = plot[n+i];
            max = i;
        }
    }

    return max;
}

unsigned pitch(const float *audio, unsigned size, CorrelationVars vars){
    std::vector<float> aa(size);
    for(unsigned i=0; i<size; i++) aa[i] = audio[i];
    return pitch(aa, vars);
}

unsigned hinted_pitch(const std::vector<float> &audio, unsigned hint, CorrelationVars vars){
    return hinted_pitch(audio.data(), audio.size(), hint, vars);
}

unsigned hinted_pitch(const float *audio, unsigned size, unsigned hint, CorrelationVars vars){
    
    if(size < 2 * hint) return hint;

    std::vector<float> left(hint), right(hint);
    for(unsigned i=0; i<hint; i++) left[i] = audio[i];
    for(unsigned i=0; i<hint; i++) right[i] = audio[hint+i];
    
    if(vars.sign){
        for(float &i : left) i = sign(i) * std::pow(std::abs(i), vars.exponent);
        for(float &i : right) i = sign(i) * std::pow(std::abs(i), vars.exponent);
    }
    else {
        for(float &i : left) i = std::pow(std::abs(i), vars.exponent);
        for(float &i : right) i = std::pow(std::abs(i), vars.exponent);
    }

    std::vector<float> c = math::correlation(left, right);

    float biggest = c[0];
    unsigned offset = 0;

    unsigned csize = c.size();

    for(unsigned i=1; i<hint; i++){

        if(c[i] > biggest){
            biggest = c[i];
            offset = i;
        }
        
        if(c[csize-i] > biggest){
            biggest = c[csize-i];
            offset = -i;
        }
    }

    return hint + offset;
}

std::vector<float> ml_graph(ml::Stack *stack, const std::vector<float> &audio){

    if(stack == nullptr || !stack->good()) return std::vector<float>(audio.size(), 3.14f);
    
    std::vector<float> clip = audio;

    float amax = 0.0f;
    for(float &i : clip) amax = std::max(amax, std::abs(i));
    if(amax != 0.0f) for(float &i : clip) i /= amax;

    int N = stack->in_size()-1;
    auto f = math::ft(audio, N/2);
    std::vector<float> freqs(N);
    for(int i=0; i<N/2; i++){
        freqs[2*i] = f[i].real();
        freqs[2*i+1] = f[i].imag();
    }
    
    freqs.push_back((float)clip.size() / 44100);
    freqs = stack->run(freqs);

    for(int i=0; i<N/2; i++) f[i] = {freqs[2*i], freqs[2*i+1]};

    clip = math::ift(f, audio.size());

    float bmax = 0.0f;
    for(float &i : clip) bmax = std::max(bmax, std::abs(i));
    for(float &i : clip) i *= amax/bmax;

    return clip;
}

}
