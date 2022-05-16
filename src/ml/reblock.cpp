#include "ml/reblock.h"

namespace ml {

void Reblock::push(){

    for(unsigned i=0, j=0, l=0, r=0;; l++, r++){
        
        if(l == left[i].size){ l = 0; i++; }
        if(r == right[j].size){ r = 0; j++; }
        if(i == left.size()) break;

        right[j][r] = left[i][l];
    }
}

void Reblock::pull(){
    
    if(nopull) return;

    for(unsigned i=0, j=0, l=0, r=0;; l++, r++){
        
        if(l == left[i].size){ l = 0; i++; }
        if(r == right[j].size){ r = 0; j++; }
        if(i == left.size()) break;

        left[i][l] = right[j][r];
    }
}

namespace Factory { extern std::string reblock; }
std::string Reblock::get_type(){ return Factory::reblock; }

bool Reblock::ok(arrays in, arrays out, args a){
    
    if(in.size() == 0 || out.size() == 0) return 0;
    
    unsigned lsize = 0, rsize = 0;
    for(auto &i : in){
        if(i.size == 0) return 0;
        lsize += i.size;
    }
    for(auto &i : out){
        if(i.size == 0) return 0;
        rsize += i.size;
    }
    
    return lsize == rsize;
}

}

