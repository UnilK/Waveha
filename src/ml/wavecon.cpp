#include "ml/wavecon.h"

#include <cmath>

namespace ml {

// wavedecon //////////////////////////////////////////////////////////////////

void Wavedecon::push(){
    
    if(std::abs(left[0](0)) == 0.0f) left[0](0) = 1.0f;

    right[0](0) = left[0](0);
    for(unsigned i=1; i<left[0].csize; i++) right[1](i-1) = left[0](i) / left[0](0);
}

void Wavedecon::pull(){
    
    // mathematically incorrect

    for(unsigned i=1; i<left[0].csize; i++) left[0](i) = right[1](i-1) / left[0](0);
    left[0](0) = right[0](0);
}

bool Wavedecon::ok(arrays in, arrays out, args a){
    if(in.size() == 1 && out.size() == 2 && out[0].size == 2
            && out[1].size + 2 == in[0].size) return 1;
    return 0;
}

namespace Factory { extern std::string wavedecon; }
std::string Wavedecon::get_type(){ return Factory::wavedecon; }

// waverecon //////////////////////////////////////////////////////////////////

void Waverecon::push(){

    right[0](0) = left[0](0);
    for(unsigned i=0; i<left[1].csize; i++) right[0](i+1) = left[0](0) * left[1](i);
}

void Waverecon::pull(){

    left[0](0) = right[0](0);
    for(unsigned i=0; i<left[1].csize; i++){
        left[1](i) = right[0](i+1) * left[0](0);
        left[0](0) += right[0](i+1) * left[1](i);
    }
}

bool Waverecon::ok(arrays in, arrays out, args a){
    if(out.size() == 1 && in.size() == 2 && in[0].size == 2
            && in[1].size + 2 == out[0].size) return 1;
    return 0;
}

namespace Factory { extern std::string waverecon; }
std::string Waverecon::get_type(){ return Factory::waverecon; }

}

