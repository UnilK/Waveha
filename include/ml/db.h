#pragma once

#include <vector>
#include <mutex>

namespace ml {

struct InputLabel { std::vector<float> input, label; };

class TrainingData {

public:

    virtual ~TrainingData();

    InputLabel get_random();
    virtual InputLabel get(size_t position);
    virtual size_t size() const;

    std::recursive_mutex mutex;

};

}
