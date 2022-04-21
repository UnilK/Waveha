#include "ml/deloc.h"

namespace ml {

void Deloc::push(){
    if(left.size() == 0) return;
    offset = rand() % left.size();
    for(unsigned i=0; i<left.size(); i++) right[i] = left[(i+offset)%left.size()];
}

void Deloc::pull(){
    if(left.size() == 0) return;
    for(unsigned i=0; i<left.size(); i++) left[(i+offset)%left.size()] = right[i];
}

bool Deloc::ok(std::vector<float> &left, std::vector<float> &right){
    return left.size() == right.size();
}

namespace Factory { extern std::string deloc; }
std::string Deloc::get_type(){ return Factory::deloc; };

}

