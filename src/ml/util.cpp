#include "ml/util.h"

namespace ml {

array::array(std::vector<float> &ref) :
    data(ref.data()), size(ref.size()), cdata((std::complex<float>*)ref.data()), csize(ref.size()/2) {}

float random_float(){ return 2.0f * (float)rand() / RAND_MAX - 1.0f; }

float sign(float x){ return (float)((x>0.0)-(x<0.0)); }

}

