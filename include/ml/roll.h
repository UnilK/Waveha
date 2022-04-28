#pragma once

#include "ml/layer.h"

namespace ml {

class Unroll : public Layer {

public:
    
    using Layer::Layer;

    void push();
    void pull();
    
    static bool ok(std::vector<float> &left, std::vector<float> &right);
    
    virtual std::string get_type();

};



class Reroll : public Layer {

public:
    
    using Layer::Layer;
    
    void push();
    void pull();
    
    static bool ok(std::vector<float> &left, std::vector<float> &right);
    
    virtual std::string get_type();

};

}

