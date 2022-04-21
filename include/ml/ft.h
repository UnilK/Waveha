#pragma once

#include "ml/layer.h"

#include <complex>

namespace ml {

class FT : public Layer {

public:

    FT(
        std::vector<float> &source,
        std::vector<float> &destination);
    
    void push();
    void pull();
    
    static bool ok(std::vector<float> &left, std::vector<float> &right);

    std::string get_type();

private:

    std::vector<std::complex<float> > freq;

};

}
