#pragma once

#include "ml/layer.h"

#include <complex>

namespace ml {

class FT : public Layer {

public:

    FT(arrays in, arrays out, args a);
    
    void push();
    void pull();
    
    static bool ok(arrays in, arrays out, args a);

    std::string get_type();

private:
    
    array &l, &r;
    std::vector<std::complex<float> > freq;

};

}
