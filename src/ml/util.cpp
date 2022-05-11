#include "ml/util.h"

namespace ml {

array::array(std::vector<float> &ref) :
    data(ref.data()), size(ref.size()) {}

float &array::operator[](unsigned i){
    return data[i];
}

std::complex<float> &array::operator()(unsigned i){
    return ((std::complex<float>*)data)[i];
}

unsigned array::csize(){ return size>>1; }

float random_float(){ return 2.0f * (float)rand() / RAND_MAX - 1.0f; }

float sign(float x){ return (float)((x>0.0)-(x<0.0)); }

}

