#include "ml/stack.h"
#include "ml/factory.h"

#include <cmath>
#include <fstream>
#include <iostream>

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
    layers.resize(types.size()-1);

    for(unsigned i=0; i<vectors.size(); i++)
        vectors[i].resize(sizes[i]);
    
    for(unsigned i=0; i<layers.size(); i++)
        layers[i] = create_layer(types[i], vectors[i], vectors[i+1]);

    judge = create_judge(types.back(), vectors.back());

    return good();
}

bool Stack::construct_from_file(std::string file){
    
    std::ifstream loader(file);
    if(!loader.good()) return 0;

    clear();

    std::vector<std::vector<unsigned> > sizes{{}};
    std::vector<std::vector<std::string> > types{{}};

    auto num = [](std::string n) -> bool {
        bool ok = 1;
        for(char i : n) ok &= std::isdigit(i);
        return ok;
    };

    std::string s;
    while(loader >> s){
        if(s == "{"){
            sizes.push_back({});
            types.push_back({});
        }
        else if(s == "}"){
            if(sizes.size() == 1) return 0;
            loader >> s;
            if(!num(s)) return 0;
            unsigned copies = std::stoul(s), n = sizes.size();
            while(copies--){
                sizes[n-2].insert(sizes[n-2].end(), sizes[n-1].begin(), sizes[n-1].end());
                types[n-2].insert(types[n-2].end(), types[n-1].begin(), types[n-1].end());
            }
            sizes.pop_back();
            types.pop_back();
        }
        else {
            if(!num(s)) return 0;
            sizes.back().push_back(std::stoul(s));
            loader >> s;
            types.back().push_back(s);
        }
    }

    construct(sizes[0], types[0]);

    loader.close();

    return good();
}

bool Stack::good(){
    
    bool ok = (layers.size() != 0) & (judge != nullptr);
    for(Layer *layer : layers) ok &= (layer != nullptr);

    return ok;
}
    
unsigned Stack::in_size(){
    if(!good()) return 0;
    return vectors[0].size();
}

unsigned Stack::out_size(){
    if(!good()) return 0;
    return vectors.back().size();
}

Layer *Stack::get_layer(unsigned index){
    return index < layers.size() ? layers[index] : nullptr;
}

std::vector<float > Stack::run(const std::vector<float> &input){

    lock.lock();

    if(!good() || input.size() != vectors[0].size()) return {};

    for(unsigned i=0; i<input.size(); i++) vectors[0][i] = input[i];
    for(Layer *layer : layers) layer->push();
    
    lock.unlock();

    return vectors.back();
}

void Stack::train(
        const std::vector<float> &input,
        const std::vector<float> &output){

    lock.lock();

    if(!good() || input.size() != vectors[0].size()
            || output.size() != vectors.back().size()) return;

    for(unsigned i=0; i<input.size(); i++) vectors[0][i] = input[i];
    for(Layer *layer : layers) layer->push();
    
    judge->feedback(output);

    for(unsigned i=layers.size(); i-- > 0;) layers[i]->pull();

    batch += 1;
    
    lock.unlock();
}

void Stack::train_program(const TrainingData &data, unsigned batchSize, unsigned batches, double speed){

    for(unsigned i=0; i<batches; i++){
        for(unsigned j=0; j<batchSize; j++){
            unsigned pick = rand()%data.size();
            train(data[pick].first, data[pick].second);
        }
        apply_changes(speed);
    }
}

Stack::TestAnalysis Stack::test(const std::vector<InputLabel > &data){

    TestAnalysis result;
    result.errors.resize(vectors.back().size(), 0.0f);

    for(auto &i : data){
        
        if(!good() || i.second.size() != vectors.back().size()
                || i.first.size() != vectors[0].size()){
            return result;
        }

        run(i.first);

        unsigned best = 0, right = 0;
        for(unsigned j=0; j<i.second.size(); j++){
            if(vectors.back()[j] > vectors.back()[best]) best = j;
            if(i.second[j] > i.second[right]) right = j;
        }
        if(best == right) result.correct += 1;

        judge->score(i.second);

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

void Stack::apply_changes(double speed){

    if(batch < 1) return;

    for(Layer *layer : layers) layer->change(speed / batch);

    batch = 0;
}

void Stack::save(ui::Saver &saver){
    
    if(!good()){
        saver.write_string("BAD");
        return;
    }

    saver.write_string("GOOD");
    
    saver.write_unsigned(vectors.size());
    
    for(auto &i : vectors) saver.write_unsigned(i.size());
    for(auto i : layers) saver.write_string(i->get_type());
    saver.write_string(judge->get_type());

    for(auto i : layers) i->save(saver);
}

void Stack::load(ui::Loader &loader){

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

    if(judge != nullptr) delete judge;
    judge = nullptr;
}

}

