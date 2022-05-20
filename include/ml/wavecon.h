#pragma once

#include "ml/layer.h"

namespace ml {

class Wavedecon : public Layer {

    /*
       deconstruct a set of frequencies to two functions:
       1. wave function (pure sine)
       2. profile
       the time-side function is an elementwise multiplication of these.
       
       (8 frequencies) => (1 sine) (7 profile frequencies)
    */

public:

    using Layer::Layer;

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

};



class Waverecon : public Layer {

    // reconstructs the frequencies from a deconstruction
    // (1 sine) (7 profile frequencies) => (8 frequencies)

public:

    using Layer::Layer;

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

};

}

