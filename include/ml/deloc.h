#pragma once

#include "ml/layer.h"

namespace ml {

class Deloc : public Layer {

public:

    Deloc(
            std::vector<float> &source,
            std::vector<float> &destination);

    void push();
    void pull();
    
    static bool ok(std::vector<float> &left, std::vector<float> &right);

    std::string get_type();

private:

    unsigned offset;

};

}

