#pragma once

#include "ml/cmatrix.h"

namespace ml {

class Fdeco : public CMatrix {

    /*
        decompose a frequency decomposition to a set of periodic functions. 
        phase is "perserved" i.e. the following hold reasonably well:
        1. The phases of the periodic functions are shifted by theta if
        the input funcion is shifted by theta.
        2. Similarly shaped functions have similar phases.
    */

public:
    
    using CMatrix::CMatrix;
    using CMatrix::ok;

    void push();
    void pull();
    
    virtual std::string get_type();

private:

    std::complex<float> phase;

};

}

