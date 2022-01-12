#include "changer/pitch.h"
#include "math/fft.h"

#include <math.h>

namespace wave {

std::vector<float> Pitch::graph(const std::vector<float> &audio){

    auto spectrum = math::fft(audio);

    std::vector<float> plot(audio.size(), 0.0f);

    for(uint32_t i = 2 * peaks; i * 2 < plot.size(); i++){
        
        float fundamental = (float)plot.size() / i;

        for(int32_t j=1; j<=peaks; j++){
            
            float frequency = fundamental * j;
            int32_t k = std::floor(frequency);
            float d  = frequency - k;

            if(k+1 > (int32_t)audio.size() / 2) break;

            // there's a phase flip on the peak so use - instead of +.
            plot[i] += std::pow(std::abs(spectrum[k] * (1 - d) - spectrum[k+1] * d), factor);

        }

    }

    return plot;
}

int32_t Pitch::period(const std::vector<float> &audio){
    
    auto plot = graph(audio);

    int32_t period = 0;
    float max = 0;

    for(uint32_t i=0; i<plot.size(); i++){
        if(max < plot[i]){
            period = i;
            max = plot[i];
        }
    }

    return period;
}

}
