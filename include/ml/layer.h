#pragma once

#include "ui/fileio.h"

#include <vector>
#include <string>

namespace ml {

class Layer {

public:

    Layer(
            std::vector<float> &source,
            std::vector<float> &destination);

    virtual ~Layer();

    // push data from left to right
    virtual void push() = 0;
    
    // pull feedback from right to left
    virtual void pull() = 0;

    // apply changes that have been accumulated in this layer
    virtual void change(double factor);

    // save and load the state of the layer to a file.
    virtual void save(ui::Saver &saver);
    virtual void load(ui::Loader &loader);
    virtual std::string get_type() = 0;

protected:
    
    std::vector<float> &left, &right;

};

}

