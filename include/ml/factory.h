#pragma once

#include <vector>
#include <complex>

namespace ml {

class Layer;

Layer *create_layer(
        std::string type, 
        std::vector<std::complex<float> > &left,
        std::vector<std::complex<float> > &right);

}

