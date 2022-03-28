#pragma once

#include <string>
#include <vector>

namespace ml {

typedef std::pair<std::vector<float>, std::vector<float> > InputLabel;

unsigned endian_flip(unsigned);

// reads file file.img and file.lb and converts them to matching pairs.
std::vector<InputLabel > mnist_input_label_pairs(std::string file);

}

