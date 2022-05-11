#pragma once

#include "ml/layer.h"

namespace ml {

    // performs a transformation to a frequency side vector
    // the resulting "frequencies" are of the form e^i(a+b+theta)
    // where theta is the phase of the function. There is
    // actually no choice of theta, what's important is that
    // the "frequencies" spin at the same speed. Similar
    // functions produce similar transforms.

class Unroll : public Layer {

public:

    Unroll(arrays in, arrays out, args);

    void push();
    void pull();
    
    static bool ok(arrays in, arrays out, args);
    
    virtual std::string get_type();

private:

    array &l, &r;

};



class Reroll : public Layer {

    // rerolls frequencies so that they correspond to
    // a fourier transform of a real function again.

public:
    
    Reroll(arrays in, arrays out, args);
    
    void push();
    void pull();
    
    static bool ok(arrays in, arrays out, args);
    
    virtual std::string get_type();

private:

    array &l, &r;

};

}

