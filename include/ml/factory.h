#pragma once

#include "ml/util.h"

#include <string>
#include <vector>

namespace ml {

typedef std::vector<array>& arrays;
typedef const std::vector<std::string>& args;

class Layer;
class Judge;

Layer *create_layer(std::string type, arrays left, arrays right, args);

Judge *create_judge(std::string type, std::vector<float> &result);

}

