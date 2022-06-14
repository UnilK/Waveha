#include "ml/norm.h"

#include <cmath>

namespace ml {

// norm ///////////////////////////////////////////////////////////////////////

Norm::Norm(arrays in, arrays out, args a) :
    Layer(in, out, a), l(left[0]), r(right[0]) {}

void Norm::push(){
    
    sum = 0.0f;
    for(unsigned i=0; i<l.size; i++) sum += std::abs(l[i]);

    sum /= 2.0f * l.size;

    float isum = 1.0f;
    if(sum != 0.0f) isum = 1.0f / sum;

    for(unsigned i=0; i<l.size; i++) r[i] = l[i] * isum;
}

void Norm::pull(){
    
    if(nopull) return;

    float isum = 1.0f, dsum = 0.0f;
    if(sum != 0.0f) isum = 1.0f / sum;
    
    for(unsigned i=0; i<l.size; i++) dsum -= l[i] * r[i] * isum * isum;

    for(unsigned i=0; i<l.size; i++) l[i] = r[i] * isum + dsum * sign(l[i]);
}

bool Norm::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1 &&
        in[0].size == out[0].size;
}

namespace Factory { extern std::string norm; };
std::string Norm::get_type(){ return Factory::norm; }

// cnorm //////////////////////////////////////////////////////////////////////

CNorm::CNorm(arrays in, arrays out, args a) :
    Layer(in, out, a), l(left[0]), r(right[0]) {}

void CNorm::push(){
    
    sum = 0.0f;
    for(unsigned i=0; i<l.csize; i++) sum += std::abs(l(i));
    
    sum /= 2.0f * l.csize;
    
    float isum = 1.0f;
    if(sum != 0.0f) isum = 1.0f / sum;

    for(unsigned i=0; i<l.csize; i++) r(i) = l(i) * isum;
}

void CNorm::pull(){

    if(nopull) return;

    float isum = 1.0f;
    if(sum != 0.0f) isum = 1.0f / sum;
    
    std::complex<float> dsum = 0.0f;
    for(unsigned i=0; i<l.csize; i++) dsum -= std::conj(l(i)) * r(i) * isum * isum;

    for(unsigned i=0; i<l.csize; i++){
        float d = std::abs(l(i));
        if(d != 0.0f) d = 1.0f / d;
        l(i) = r(i) * isum + l(i) * d * dsum;
    }
}

bool CNorm::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1 &&
        in[0].size == out[0].size && in[0].size % 2 == 0;
}

namespace Factory { extern std::string cnorm; };
std::string CNorm::get_type(){ return Factory::cnorm; }

}

