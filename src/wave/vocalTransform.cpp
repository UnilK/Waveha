#include "wave/vocalTransform.h"

#include <algorithm>

VocalTransform::VocalTransform(){
    this->set_size(90);
}

VocalTransform::VocalTransform(int32_t size_){
    this->set_size(size_);
}

void VocalTransform::set_size(int32_t size_){
    this->size = size_;
    std::vector<std::complex<float> > tmp(this->size, {0.0f, 0.0f});
    this->trans.resize(this->size, tmp);

    for(int32_t i=0; i<this->size; i++){
        trans[i][i] = {1.0f, 0.0f};
    }
}

int32_t VocalTransform::get_size(){ return this->size; }

void VocalTransform::set_fundamental_shift(float fundShift_){ this->fundShift = fundShift_; }

float VocalTransform::get_fundamental_shift(){ return this->fundShift; }

void VocalTransform::set_value(int32_t from, int32_t to, std::complex<float> value){
    if(from >= this->size || from < 0) return;
    if(to >= this->size || to < 0) return;
    this->trans[from][to] = value;
}

void VocalTransform::set_transform(std::vector<std::vector<std::complex<float> > > trans_){
    
    if(!trans_.size() || trans_.size() != trans_[0].size()) return;
    this->size = trans_.size();
    this->trans = trans_;

}

Vocal VocalTransform::transform(Vocal source){
    
    int32_t minSize = std::min(source.get_size(), this->size);
    Vocal formed(minSize, source.get_fundamental()*this->fundShift);

    for(int32_t to=0; to<minSize; to++){
        std::complex<float> total = {0, 0};
        for(int32_t from=0; from<minSize; from++){
            total += this->trans[to][from]*source.get(from);
        }
        formed.set(to, total);
    }

    return formed;
}
