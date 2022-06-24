#pragma once

#include "ml/layer.h"

namespace ml {

class Dephase : public Layer {

public:
    
    using Layer::Layer;

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

};



class Rephase : public Layer {

public:
    
    using Layer::Layer;

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

};

}

