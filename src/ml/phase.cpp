#include "ml/phase.h"

namespace ml {

// dephase ////////////////////////////////////////////////////////////////////

void Dephase::push(){
    
    for(unsigned i=0; i<left[0].csize; i++) right[1][i] = std::abs(left[0](i));
    for(unsigned i=0; i<left[0].csize; i++){
        right[0](i) = right[1][i] == 0.0f ? left[0](i) : left[0](i) / right[1][i];
    }
}

void Dephase::pull(){

    if(nopull) return;
    
    for(unsigned i=0; i<right[0].csize; i++) left[0](i) = right[0](i)*right[1][i];
}

namespace Factory { extern std::string dephase; }
std::string Dephase::get_type(){ return Factory::dephase; }

bool Dephase::ok(arrays in, arrays out, args a){
    if(in.size() != 1 || out.size() != 2 || out[0].size != in[0].size ||
            out[1].size * 2 != out[0].size ) return 0;
    return 1;
}

// rephase ////////////////////////////////////////////////////////////////////

void Rephase::push(){
    for(unsigned i=0; i<left[0].csize; i++) right[0](i) = left[0](i)*left[1][i];
}

void Rephase::pull(){

    if(nopull) return;

    for(unsigned i=0; i<right[0].csize; i++){
        left[1][i] = left[0](i).real() * right[0](i).real() +
            left[0](i).imag() * right[0](i).imag();
    }
    for(unsigned i=0; i<right[0].csize; i++){
        float d = std::abs(right[0](i));
        left[0](i) = d == 0.0f ? right[0](i) : right[0](i) / d;
    }
}

namespace Factory { extern std::string rephase; }
std::string Rephase::get_type(){ return Factory::rephase; }

bool Rephase::ok(arrays in, arrays out, args a){
    if(out.size() != 1 || in.size() != 2 || out[0].size != in[0].size ||
            in[1].size * 2 != in[0].size ) return 0;
    return 1;
}

}

