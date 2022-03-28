#include "ml/stack.h"
#include "ml/factory.h"

#include <cmath>
#include <fstream>

namespace ml {

Stack::Stack(){}

Stack::Stack(std::vector<unsigned> sizes, std::vector<std::string> types){
    construct(sizes, types);    
}

Stack::~Stack(){
    clear();
}

bool Stack::construct(std::vector<unsigned> sizes, std::vector<std::string> types){
    
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

bool Stack::construct_from_file(std::string file){
    
    std::ifstream loader(file);
    if(!loader.good()) return 0;

    clear();

    unsigned size;
    loader >> size;

    std::vector<unsigned> sizes(size);
    std::vector<std::string> types(size);

    for(unsigned i=0; i<size; i++) loader >> sizes[i] >> types[i];

    construct(sizes, types);

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

std::vector<float > Stack::run(const std::vector<float> &input){

    if(!good() || input.size() != vectors[0].size()) return {};

    for(unsigned i=0; i<input.size(); i++) vectors[0][i] = input[i];
    for(Layer *layer : layers) layer->push();
    
    return vectors.back();
}

void Stack::train(
        const std::vector<float> &input,
        const std::vector<float> &output){
   
    if(!good() || input.size() != vectors[0].size()
            || output.size() != vectors.back().size()) return;

    for(unsigned i=0; i<input.size(); i++) vectors[0][i] = input[i];
    for(Layer *layer : layers) layer->push();
    
    for(unsigned i=0; i<output.size(); i++){
        vectors.back()[i] = output[i] - vectors.back()[i];
    }
    evaluate->push();

    for(unsigned i=layers.size(); i-- > 0;) layers[i]->pull();

    batch += 1;
}

void Stack::train_progam(const std::vector<InputLabel > &data, unsigned batches){

    for(unsigned i=0; i<batches; i++){
        for(unsigned j=0; j<batchSize; j++){
            unsigned pick = rand()%data.size();
            train(data[pick].first, data[pick].second);
        }
        apply_changes();
    }
}

Stack::TestAnalysis Stack::test(const std::vector<InputLabel > &data){

    TestAnalysis result;
    result.errors.resize(vectors.back().size(), 0.0f);

    for(auto &i : data){
        
        run(i.first);
        
        if(!good() || i.second.size() != vectors.back().size()) return result;
        
        for(unsigned j=0; j<i.second.size(); j++){
            vectors.back()[j] = i.second[j] - vectors.back()[j];
        }

        evaluate->push();

        for(unsigned j=0; j<i.second.size(); j++){
            result.errors[j] += std::abs(vectors.back()[j]);
        }
    }

    for(double &i : result.errors){
        i /= data.size();
        result.error += i;
    }

    return result;
}

void Stack::set_batch_size(unsigned size){ batchSize = size; }

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

    construct(sizes, types);

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

