#pragma once

#include "app/session.h"

#include <vector>
#include <complex>
#include <map>
#include <functional>

namespace ml {

class Layer {

public:

    Layer(
            std::vector<std::complex<float> > &source,
            std::vector<std::complex<float> > &destination);

    virtual ~Layer();

    // push data from left to right
    virtual void push() = 0;
    
    // pull feedback from right to left
    virtual void pull() = 0;

    // apply changes that have been accumulated in this layer
    virtual void change(double factor);

    // save and load state of layer to a file.
    virtual void save(app::Saver &saver);
    virtual void load(app::Loader &loader);
    virtual std::string get_type() = 0;

protected:
    
    std::vector<std::complex<float> > &left, &right;

};

}

