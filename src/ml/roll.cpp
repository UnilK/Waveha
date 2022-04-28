#include "ml/roll.h"

#include <complex>

namespace ml {

// unroll /////////////////////////////////////////////////////////////////////

void Unroll::push(){

    // 0 hecks given.

    unsigned n = left.size()/2;
    std::complex<float> *l = (std::complex<float>*)left.data();
    std::complex<float> *r = (std::complex<float>*)right.data();

    if(n == 0) return;

    r[0] = l[0];
    for(unsigned i=1; i<n; i++){
        if(std::abs(l[i-1]) != 0.0f) r[i] = l[i] / l[i-1] * std::abs(l[i-1]);
        else r[i] = l[i];
    }
}

void Unroll::pull(){

    // probably 100% incorrect. This will be placed at the front so the pull isn't used.

    unsigned n = left.size()/2;
    std::complex<float> *l = (std::complex<float>*)left.data();
    std::complex<float> *r = (std::complex<float>*)right.data();

    if(n == 0) return;

    for(unsigned i=n-1; i>0; i--){
        if(std::abs(l[i-1]) != 0.0f) l[i] = r[i] * l[i-1] / std::abs(l[i-1]);
        else r[i] = l[i];
    }
    l[0] = r[0];
}

bool Unroll::ok(std::vector<float> &left, std::vector<float> &right){
    return left.size() == right.size() && left.size() % 2 == 0;
}

namespace Factory { extern std::string unroll; }
std::string Unroll::get_type(){ return Factory::unroll; }

// reroll /////////////////////////////////////////////////////////////////////

void Reroll::push(){

    // still 0 hecks given.

    unsigned n = left.size()/2;
    std::complex<float> *l = (std::complex<float>*)left.data();
    std::complex<float> *r = (std::complex<float>*)right.data();

    if(n == 0) return;

    for(unsigned i=0; i<n; i++){
        float d = std::abs(l[i]);
        float a = std::arg(l[i]);
        r[i] = std::polar(d, a*(i+1));
    }
}

void Reroll::pull(){
    
    unsigned n = left.size()/2;
    std::complex<float> *l = (std::complex<float>*)left.data();
    std::complex<float> *r = (std::complex<float>*)right.data();

    if(n == 0) return;

    for(unsigned i=0; i<n; i++){
        float a = std::arg(l[i]);
        l[i] = std::polar(1.0f, -a*i) * r[i] / (float)(i+1);
    }
}

bool Reroll::ok(std::vector<float> &left, std::vector<float> &right){
    return left.size() == right.size() && left.size() % 2 == 0;
}

namespace Factory { extern std::string reroll; }
std::string Reroll::get_type(){ return Factory::reroll; }

}

