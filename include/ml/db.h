#pragma once

#include <vector>

namespace ml {

struct InputLabel { std::vector<float> input, label; };

class TrainingData {

public:

    virtual ~TrainingData();

    virtual InputLabel get_random();
    virtual InputLabel get_next();
    virtual void go_to(size_t position = 0);
    virtual size_t get_size();

};

}
