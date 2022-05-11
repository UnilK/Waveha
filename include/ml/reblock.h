#pragma once

#include "ml/layer.h"

namespace ml {

class Reblock : public Layer {

    // chages vector blocking. for example:
    // .... .. ...... => .. ..........

public:

    using Layer::Layer;

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

};

}

