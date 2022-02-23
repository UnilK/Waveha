#include "ml/stack.h"
#include "ml/factory.h"

#include <cmath>

namespace ml {

Stack::Stack(){}

Stack::Stack(std::vector<unsigned> sizes, std::vector<std::string> types){
    open(sizes, types);    
}

Stack::~Stack(){
    clear();
}

bool Stack::open(std::vector<unsigned> sizes, std::vector<std::string> types){
    
    clear();
    
    if(sizes.size() != types.size() || sizes.empty()) return 0;

    vectors.resize(sizes.size());
    layers.resize(types.size());

    for(unsigned i=0; i<vectors.size(); i++)
        vectors[i].resize(sizes[i]);
    
    for(unsigned i=0; i<types.size() - 1; i++)
        layers[i] = create_layer(types[i], vectors[i], vectors[i+1]);

    evaluate = create_layer(types.back(), vectors.back(), vectors.back());

    return good();
}

bool Stack::good(){
    
    bool ok = layers.size() != 0;
    ok &= evaluate != nullptr;
    for(Layer *layer : layers) ok &= layer != nullptr;

    return ok;
}

Layer *Stack::get_layer(unsigned index){
    return index < layers.size() ? layers[index] : nullptr;
}

std::vector<std::complex<float> > Stack::run(const std::vector<std::complex<float> > &input){

    if(!good() || input.size() != vectors[0].size()) return {};

    for(unsigned i=0; i<input.size(); i++) vectors[0][i] = input[i];
    for(Layer *layer : layers) layer->push();
    
    return vectors.back();
}

void Stack::train(
        const std::vector<std::complex<float> > &input,
        const std::vector<std::complex<float> > &output){
   
    if(!good() || input.size() != vectors[0].size() || output.size() != vectors.back().size()) return;

    for(unsigned i=0; i<input.size(); i++) vectors[0][i] = input[i];
    for(Layer *layer : layers) layer->push();

    evaluate->push();

    for(unsigned i=layers.size(); i-- > 0;) layers[i]->pull();

    batch += 1;
}

double Stack::score(
        const std::vector<std::complex<float> > &input,
        const std::vector<std::complex<float> > &output){

    if(!good() || input.size() != vectors[0].size() || output.size() != vectors.back().size()) return 0;

    for(unsigned i=0; i<input.size(); i++) vectors[0][i] = input[i];
    for(Layer *layer : layers) layer->push();

    evaluate->push();

    double total = 0;

    for(auto i : vectors.back()) total += std::abs(i);

    return total;
}

void Stack::set_speed(double s){ speed = s; }

void Stack::apply_changes(){

    if(batch < 1) return;

    for(Layer *layer : layers) layer->change(speed / batch);

    batch = 0;
}

void Stack::save(app::Saver &saver){
    
    if(!good()){
        saver.write_string("BAD");
        return;
    }

    saver.write_string("GOOD");
    
    saver.write_unsigned(layers.size());
    
    for(auto &i : vectors) saver.write_unsigned(i.size());
    for(auto i : layers) saver.write_string(i->get_type());
    
    for(auto i : layers) i->save(saver);
}

void Stack::load(app::Loader &loader){

    clear();

    if(loader.read_string() == "BAD") return;

    unsigned size = loader.read_unsigned();

    std::vector<unsigned> sizes(size);
    std::vector<std::string> types(size);

    for(auto &i : sizes) i = loader.read_unsigned();
    for(auto &i : types) i = loader.read_string();

    open(sizes, types);

    for(auto i : layers) i->load(loader);

}

void Stack::clear(){
    
    vectors.clear();
    
    for(Layer *layer : layers) if(layer != nullptr) delete layer;
    layers.clear();

    if(evaluate != nullptr) delete evaluate;
    evaluate = nullptr;
}

}

