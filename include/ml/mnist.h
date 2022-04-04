#pragma once

#include <string>
#include <vector>

namespace ml {

typedef std::pair<std::vector<float>, std::vector<float> > InputLabel;
typedef std::vector<InputLabel > TrainingData;

unsigned endian_flip(unsigned);

// reads file file.img and file.lb and converts them to matching pairs.
TrainingData *mnist_data(std::string file);

}
