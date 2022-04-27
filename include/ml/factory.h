#pragma once

#include <string>
#include <vector>

namespace ml {

class Layer;
class Judge;

Layer *create_layer(
        std::string type, 
        std::vector<float> &left,
        std::vector<float> &right);

Judge *create_judge(
        std::string type,
        std::vector<float> &result);

}

