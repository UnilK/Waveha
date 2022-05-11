#include "ml/deloc.h"

namespace ml {

void Deloc::push(){

    if(left[0].size == 0) return;
    
    offset = rand() % left[0].size;
    for(unsigned i=0; i<left[0].size; i++) right[0][i] = left[0][(i+offset)%left[0].size];
}

void Deloc::pull(){

    if(nopull) return;

    if(left[0].size == 0) return;

    for(unsigned i=0; i<left[0].size; i++) left[0][(i+offset)%left[0].size] = right[0][i];
}

bool Deloc::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1 && in[0].size == out[0].size;
}

namespace Factory { extern std::string deloc; }
std::string Deloc::get_type(){ return Factory::deloc; };

}

