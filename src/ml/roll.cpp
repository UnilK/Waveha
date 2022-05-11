#include "ml/roll.h"

#include <complex>

namespace ml {

// unroll /////////////////////////////////////////////////////////////////////

Unroll::Unroll(arrays in, arrays out, args a) :
    Layer(in, out, a), l(left[0]), r(right[0]) {}

void Unroll::push(){

    r.cdata[0] = l.cdata[0];
    for(unsigned i=1; i<l.csize; i++){
        float d = std::abs(l.cdata[i-1]);
        if(d != 0.0f) r.cdata[i] = l.cdata[i] * std::conj(l.cdata[i-1]) / d;
        else r.cdata[i] = l.cdata[i];
    }
}

void Unroll::pull(){

    if(nopull) return;

    // NOP. This will be placed at the front so the pull isn't used.
}

bool Unroll::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1 &&
        in[0].size == out[0].size && in[0].size % 2 == 0;
}

namespace Factory { extern std::string unroll; }
std::string Unroll::get_type(){ return Factory::unroll; }

// reroll /////////////////////////////////////////////////////////////////////

Reroll::Reroll(arrays in, arrays out, args a) :
    Layer(in, out, a), l(left[0]), r(right[0]) {}

void Reroll::push(){

    for(unsigned i=0; i<l.csize; i++){
        float d = std::abs(l.cdata[i]);
        float a = std::arg(l.cdata[i]);
        r.cdata[i] = std::polar(d, a*(i+1));
    }
}

void Reroll::pull(){
    
    for(unsigned i=0; i<l.csize; i++){
        
        // mathematically incorrect, but works ok and intuitively makes some sense.

        /*
        if(std::abs(l.cdata[i]) != 0.0f){
            float a = std::arg(l.cdata[i]);
            l.cdata[i] = std::polar(1.0f, -a*i) * r.cdata[i] / (float)(i+1);
        }
        */
        
        // this one should be mathematically correct and performs ok on
        // lower mutation rates - the higher frequencies spin fast as heck.

        if(std::abs(l.cdata[i]) != 0.0f){
            
            float a = std::arg(l.cdata[i]);
            auto d = std::polar(1.0f, -a*i) * r.cdata[i];
            
            std::complex<float> norm = l.cdata[i] / std::abs(l.cdata[i]);
            std::complex<float> perp = {-norm.imag(), norm.real()};
            
            float dn = d.real() * norm.real() + d.imag() * norm.imag();
            float dp = d.real() * perp.real() + d.imag() * perp.imag();

            l.cdata[i] = norm * dn + perp * dp * (float)(i+1);
        }
        
    }
}

bool Reroll::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1 &&
        in[0].size == out[0].size && in[0].size % 2 == 0;
}

namespace Factory { extern std::string reroll; }
std::string Reroll::get_type(){ return Factory::reroll; }

}

