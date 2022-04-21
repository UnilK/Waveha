#pragma once

#include "ml/cmatrix.h"

namespace ml {

class Freco : public CMatrix {

    /*
        recompose a frequency decomposition of a
        periodic funtion form a set of other periodic functions.
    */

public:
    
    using CMatrix::CMatrix;
    using CMatrix::ok;

    void push();
    void pull();
    
    virtual std::string get_type();

};

}

