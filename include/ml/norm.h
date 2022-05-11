#pragma once

#include "ml/layer.h"

namespace ml {

class Norm : public Layer {

    // normalizes average absolute value to 1.0.

public:

    Norm(arrays in, arrays out, args a);

    void push();
    void pull();
    
    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

private:

    array &l, &r;
    float sum;

};



class CNorm : public Layer {

    // normalizes average absolute value to 1.0.

public:

    CNorm(arrays in, arrays out, args a);
    
    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

private:

    array &l, &r;
    float sum;

};

}

