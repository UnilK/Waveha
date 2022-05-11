#pragma once

#include "ml/layer.h"

namespace ml {

    // "delocalizes" a vector by cyclically shifting it randomly

class Deloc : public Layer {

public:
    
    using Layer::Layer;

    void push();
    void pull();
    
    static bool ok(arrays in, arrays out, args);

    std::string get_type();

private:

    unsigned offset;

};

}

