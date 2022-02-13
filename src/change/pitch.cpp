#include "change/pitch.h"
#include "math/fft.h"

#include <math.h>

namespace change {

std::vector<float> Pitch::graph(const std::vector<float> &audio){

    std::vector<float> copy = audio;
    for(auto &i : copy) i = i*i*i; 
    auto spectrum = math::fft(copy);

    std::vector<float> plot(audio.size(), 0.0f);

    for(unsigned i = 2 * peaks; i * 2 < plot.size(); i++){
        
        float fundamental = (float)plot.size() / i;

        for(int j=1; j<=peaks; j++){
            
            float frequency = fundamental * j;
            int k = std::floor(frequency);
            float d  = frequency - k;

            if(k+1 > (int)audio.size() / 2) break;

            // there's a phase flip on the peak so use - instead of +.
            plot[i] += std::pow(std::abs(spectrum[k] * (1 - d) - spectrum[k+1] * d), factor);

        }

    }

    return plot;
}

int Pitch::period(const std::vector<float> &audio){
    
    auto plot = graph(audio);

    int period = 0;
    float max = 0;

    for(unsigned i=0; i<plot.size(); i++){
        if(max < plot[i]){
            period = i;
            max = plot[i];
        }
    }

    return period;
}

}
