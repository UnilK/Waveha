#pragma once

#include <string>
#include <vector>

namespace ml {

class Layer;

Layer *create_layer(
        std::string type, 
        std::vector<float> &left,
        std::vector<float> &right);

}

