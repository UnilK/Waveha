#pragma once

#include <vector>
#include <mutex>

namespace ml {

struct InputLabel { std::vector<float> input, label; };

class TrainingData {

public:

    virtual ~TrainingData();

    InputLabel get_random();
    InputLabel get(size_t position);
    virtual InputLabel direct_get(size_t position);
    virtual size_t size() const;

    void switch_to_cache();
    void clear_cache();

    std::recursive_mutex mutex;

protected:

    bool cached = 0;
    std::vector<InputLabel> cache;

};

}
