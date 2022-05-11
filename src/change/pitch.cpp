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
    std::reverse(reverseCopy.begin(), reverseCopy.end());

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

std::vector<float> ml_graph(ml::Stack *stack, const std::vector<float> &audio){

    if(stack == nullptr || !stack->good()) return std::vector<float>(audio.size(), 3.14f);
    
    std::vector<float> clip = audio;
            
    int N = stack->in_size();
    auto f = math::ft(audio, N/2);
    std::vector<float> freqs(N);
    for(int i=0; i<N/2; i++){
        freqs[2*i] = f[i].real();
        freqs[2*i+1] = f[i].imag();
    }
    
    float sum = 0.0f;
    for(auto &i : f) sum += std::abs(i);
    sum /= f.size();
    
    freqs = stack->run(freqs);

    for(int i=0; i<N/2; i++) f[i] = {freqs[2*i], freqs[2*i+1]};
    
    for(auto &i : f) i *= sum;

    clip = math::ift(f, audio.size());

    return clip;
}

}
