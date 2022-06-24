#pragma once

#include "ml/layer.h"

namespace ml {

class Multiply : public Layer {

    // multiplies one vector with another. The first vector can be set as static
    // so that pulls don't affect it.
    // size 4 X size 3 => size 12 (3 3 3 3)

public:

    Multiply(arrays in, arrays out, args);

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

private:

    bool fnopull = 0;

};



class CMultiply : public Layer {

    // same but for complex numbers

public:

    CMultiply(arrays in, arrays out, args);

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

private:

    bool fnopull = 0, abs = 0;

};



class PMultiply : public Layer {

    // element-wise multiply two vectors

public:

    using Layer::Layer;

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

};

}

