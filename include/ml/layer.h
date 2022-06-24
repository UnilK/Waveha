#pragma once

#include "ui/fileio.h"
#include "ml/util.h"

#include <vector>
#include <string>

namespace ml {

typedef std::vector<array>& arrays;
typedef const std::vector<std::string>& args;

class Layer {

public:

    // left is in, right is out
    Layer(arrays in, arrays out, args);

    virtual ~Layer();

    // push data from left to right
    virtual void push() = 0;
    
    // pull feedback from right to left
    virtual void pull() = 0;

    // get sum of variable change squares and the number of variables.
    // useful for calculating the change vector length for a whole
    // network in an euclidean space.
    virtual std::pair<float, unsigned> sum_change();
    
    // apply changes that have been accumulated in this layer
    virtual void change(float factor, float decay);

    // save and load the state of the layer to a file.
    virtual void save(ui::Saver &saver);
    virtual void load(ui::Loader &loader);
    
    virtual std::string get_type() = 0;
    static bool ok(arrays in, arrays out, args a);

protected:
    
    // left is in, right is out
    std::vector<array> left, right;
    bool nopull = 0;

};



class Sink : public Layer {

public:
    
    using Layer::Layer;

    void push();
    void pull();
    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

};

}

