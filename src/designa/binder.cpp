#include "designa/binder.h"

#include <cmath>
#include <cassert>

namespace designa {

Binder::Binder(){
    bound = 1.0f;
}

float Binder::process(float sample){
    if(std::abs(sample) > bound){

        float sign = (sample > 0.0f) - (sample < 0.0f);
        float amplitude = sign * sample;
        
        amplitude = 1.0f - (1.0f - bound) * (1.0f - bound) / (amplitude - 2 * bound + 1);
        sample = sign * amplitude;
    }

    return sample;
}

void Binder::set_bound(float b){
    assert(b >= 0.0f && b <= 1.0f);
    bound = b;
}

}

