#include "ml/norm.h"

#include <cmath>

namespace ml {

// norm ///////////////////////////////////////////////////////////////////////

Norm::Norm(arrays in, arrays out, args a) :
    Layer(in, out, a), l(left[0]), r(right[0]) {}

void Norm::push(){
    
    sum = 0.0f;
    for(unsigned i=0; i<l.size; i++) sum += std::abs(l.data[i]);
    if(sum != 0.0f) sum = l.size / sum;

    for(unsigned i=0; i<l.size; i++) r.data[i] = l.data[i] * sum;
}

void Norm::pull(){
    
    // mathematically incorrect. Try correct one later.

    if(nopull) return;

    for(unsigned i=0; i<l.size; i++) l.data[i] = r.data[i] * sum;
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
    for(unsigned i=0; i<l.csize; i++) sum += std::abs(l.cdata[i]);
    if(sum != 0.0f) sum = l.csize / sum;

    for(unsigned i=0; i<l.csize; i++) r.cdata[i] = l.cdata[i] * sum;
}

void CNorm::pull(){
    
    // mathematically incorrect

    if(nopull) return;

    for(unsigned i=0; i<l.csize; i++) l.cdata[i] = r.cdata[i] * sum;
}

bool CNorm::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1 &&
        in[0].size == out[0].size && in[0].size % 2 == 0;
}

namespace Factory { extern std::string cnorm; };
std::string CNorm::get_type(){ return Factory::cnorm; }

}

