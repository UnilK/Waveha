#include "ml/layer.h"

#include <assert.h>

namespace ml {

// layer //////////////////////////////////////////////////////////////////////

Layer::Layer(arrays in, arrays out, args a) : left(in), right(out) {
    for(auto &i : a) if(i == "nopull") nopull = 1;
}

Layer::~Layer(){}

void Layer::change(float factor, float decay){}

void Layer::save(ui::Saver &saver){}

void Layer::load(ui::Loader &loader){}

bool Layer::ok(arrays in, arrays out, args a){ return 1; }

// sink ///////////////////////////////////////////////////////////////////////

void Sink::push(){}

void Sink::pull(){}

namespace Factory { extern std::string sink; }
std::string Sink::get_type(){ return Factory::sink; }

bool Sink::ok(arrays in, arrays out, args a){ return 1; }

}

