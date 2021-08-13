#include "wave/vocalTransform.h"

#include <algorithm>

namespace wave{

VocalTransform::VocalTransform(){
    set_size(1);
}

VocalTransform::VocalTransform(int32_t size_){
    set_size(size_);
}

void VocalTransform::set_size(int32_t size_){
    size = size_;
    std::vector<std::complex<float> > tmp(size, {0.0f, 0.0f});
    trans.resize(size, tmp);

    for(int32_t i=0; i<size; i++){
        trans[i][i] = {1.0f, 0.0f};
    }
}

int32_t VocalTransform::get_size(){ return size; }

void VocalTransform::set_fundamental_shift(float fundShift_){ fundShift = fundShift_; }

float VocalTransform::get_fundamental_shift(){ return fundShift; }

void VocalTransform::set_value(int32_t from, int32_t to, std::complex<float> value){
    if(from >= size || from < 0) return;
    if(to >= size || to < 0) return;
    trans[from][to] = value;
}

void VocalTransform::set_transform(std::vector<std::vector<std::complex<float> > > trans_){
    
    if(!trans_.size() || trans_.size() != trans_[0].size()) return;
    size = trans_.size();
    trans = trans_;

}

Vocal VocalTransform::transform(Vocal source){
    
    int32_t minSize = std::min(source.get_size(), size);
    Vocal formed(minSize, source.get_fundamental()*fundShift);

    for(int32_t to=0; to<minSize; to++){
        std::complex<float> total = {0, 0};
        for(int32_t from=0; from<minSize; from++){
            total += trans[to][from]*source.get(from);
        }
        formed.set(to, total);
    }

    return formed;
}

}

