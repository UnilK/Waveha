#pragma once

#include <string>
#include <vector>

namespace ml {

typedef std::pair<std::vector<float>, std::vector<float> > InputLabel;
typedef std::vector<InputLabel > TrainingData;

int create_wave_data(std::string directory, std::string output, unsigned N);

TrainingData *wave_data(std::string file);

}

