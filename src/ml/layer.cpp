#include "ml/layer.h"

#include <assert.h>

namespace ml {


Layer::Layer(
        std::vector<float> &source,
        std::vector<float> &destination) :
    left(source),
    right(destination)
{}

Layer::~Layer(){}

void Layer::change(double factor){}

void Layer::save(ui::Saver &saver){}

void Layer::load(ui::Loader &loader){}

}

