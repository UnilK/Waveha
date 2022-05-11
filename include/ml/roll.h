#pragma once

#include "ml/layer.h"

namespace ml {

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

