#include "designg/binder.h"

#include <cmath>
#include <cassert>

namespace designg {

Binder::Binder(){
    bound = 1.0f;
}

float Binder::process(float sample){
    
    if(std::abs(sample) > bound){

        float sign = (sample > 0.0f) - (sample < 0.0f);
        double amplitude = sign * sample;
        
        amplitude = 1.0 - (1.0 - bound) * (1.0 - bound) / (amplitude - 2 * bound + 1);
        sample = sign * amplitude;
    }

    return sample;
}

void Binder::set_bound(float b){
    assert(b >= 0.0f && b <= 1.0f);
    bound = b;
}

int Binder::get_delay(){ return 0; }

}

