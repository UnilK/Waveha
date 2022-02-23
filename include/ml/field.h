#pragma once

#include "ml/layer.h"

namespace ml {

class Field : public Layer {
    
public:

    Field(
            std::vector<std::complex<float> > &source,
            std::vector<std::complex<float> > &destination);

    void push();
    void pull();

    std::string get_type();

    static const std::string type;

};

}

