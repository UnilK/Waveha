#include "ml/layer.h"

#include <assert.h>

namespace ml {

Layer::Layer(arrays in, arrays out, args a) : left(in), right(out) {
    for(auto &i : a) if(i == "nopull") nopull = 1;
}

Layer::~Layer(){}

void Layer::change(double factor){}

void Layer::save(ui::Saver &saver){}

void Layer::load(ui::Loader &loader){}

bool Layer::ok(arrays in, arrays out, args a){ return 1; }

}

