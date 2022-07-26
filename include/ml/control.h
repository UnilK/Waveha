#pragma once

#include "ml/layer.h"

namespace ml {

class Control : public Layer {

    // control unit. Multiplies and adds a bias to all values.

public:

    Control(arrays in, arrays out, args);

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

    std::string get_name();
    void set(float m, float b);
    void set_back();

private:

    std::string name;
    float defaultMul, defaultBias;
    float mul, bias;

};



class PitchLog : public Layer {

public:

    PitchLog(arrays in, arrays out, args);

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

private:

    float add, mul;

};

}
