#include "ml/layer.h"

#include <assert.h>

namespace ml {


Layer::Layer(
        std::vector<std::complex<float> > &source,
        std::vector<std::complex<float> > &destination) :
    left(source),
    right(destination)
{}

Layer::~Layer(){}

void Layer::change(double factor){}

void Layer::save(app::Saver &saver){}

void Layer::load(app::Loader &loader){}

}

