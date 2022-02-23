#include "change/pitch.h"
#include "math/fft.h"

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

}
