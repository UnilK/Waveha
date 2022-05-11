#include "ml/ft.h"
#include "math/ft.h"

namespace ml {

FT::FT(arrays in, arrays out, args a) :
    Layer(in, out, a), l(in[0]), r(out[0]), freq(r.csize())
{}

void FT::push(){

    freq = math::ft(l.data, l.size, r.csize());
    for(unsigned i=0; i<freq.size(); i++) r(i) = freq[i];
}

void FT::pull(){
    
    if(nopull) return;

    for(unsigned i=0; i<freq.size(); i++) freq[i] = r(i);
    std::vector<float> rev = math::ift(freq, l.size);
    for(unsigned i=0; i<l.size; i++) l[i] = rev[i];
}

bool FT::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1 && out[0].size % 2 == 0;
}

namespace Factory { extern std::string ft; };
std::string FT::get_type(){ return Factory::ft; }

}

