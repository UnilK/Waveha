#include "ml/stack.h"
#include "ml/factory.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <set>

using std::cout;

namespace ml {

Stack::Stack(){}

Stack::Stack(
        std::vector<std::vector<std::string> > trans,
        std::vector<std::tuple<std::string, std::string, unsigned> > links){
    construct(trans, links);
}

Stack::~Stack(){
    clear();
}

bool Stack::construct(
        std::vector<std::vector<std::string> > trans,
        std::vector<std::tuple<std::string, std::string, unsigned> > links){
    
    clear();

    build_trans = trans;
    build_links = links;

    auto type = [](std::vector<std::string> &layer){ return layer.back(); };

    auto lowtype = [](std::vector<std::string> &layer){ return layer[0]; };
    
    auto args = [](std::vector<std::string> &layer){
        std::vector<std::string> a;
        for(int i=1; i<(int)layer.size()-2; i++) a.push_back(layer[i]);
        return a;
    };
    
    auto name = [](std::vector<std::string> &layer){ return layer[layer.size()-2]; };

    // setup graph
    std::map<std::string, std::vector<std::pair<std::string, int> > > ins, outs;
    std::map<std::string, int> incount;
    std::set<std::string> judges;

    for(auto &i : trans){
        if(type(i) == "judge"){
            judges.insert(name(i));
            continue;
        }
        if(incount.count(name(i))){
            std::cout << "8\n";
            return 0;
        }
        incount[name(i)] = 0;
    }

    if(judges.size() != 1){
        std::cout << "9\n";
        return 0;
    }

    // set input vector as first and ouput as last
    for(unsigned i=0; i<links.size(); i++){
        auto [in, out, size] = links[i];
        if(judges.count(out)){
            std::swap(links.back(), links[i]);
        }
        if(i != 0 && !incount.count(in)){
            if(!incount.count(in)){
                std::cout << in << ' ' << out << " 10\n";
                return 0;
            }
            std::swap(links.front(), links[i]);
        }
    }

    // edges
    for(auto [in, out, size] : links){
        vectors.push_back(std::vector<float>(size, 0.0f));
        outs[in].push_back({out, vectors.size()-1});
        ins[out].push_back({in, vectors.size()-1});
        if(incount.count(in) && incount.count(out)) incount[out]++;
    }

    // topological order for layers
    std::vector<std::vector<std::string> > topo;
    std::map<std::string, std::vector<std::string> > nameindex;

    for(auto &i : trans){
        if(type(i) == "judge"){
            judge = create_judge(lowtype(i), vectors.back());
            continue;
        }
        nameindex[name(i)] = i;
        if(incount[name(i)] == 0) topo.push_back(i);
    }

    for(unsigned i=0; i<topo.size(); i++){
        
        std::string node = name(topo[i]);
        
        std::vector<array> in, out;

        for(auto &[f, s] : ins[node]){
            in.emplace_back(vectors[s]);
        }

        for(auto &[f, s] : outs[node]){
            incount[f]--;
            if(incount[f] == 0) topo.push_back(nameindex[f]);
            out.emplace_back(vectors[s]);
        }
        
        layers.push_back(create_layer(lowtype(topo[i]), in, out, args(topo[i])));
        /*
        std::cout << "# ";
        for(auto j : topo[i]) std::cout << j << ' ';
        std::cout << '\n';
        std::cout << "in:\n";
        for(auto [f, s] : ins[node]) std::cout << f << ' ' << s << '\n';
        std::cout << "out:\n";
        for(auto [f, s] : outs[node]) std::cout << f << ' ' << s << '\n';
        if(layers.back() == nullptr) cout << "BAD\n";
        std::cout << '\n';
        */
    }

    return good();
}

bool Stack::construct_from_file(std::string file){
    
    std::ifstream loader(file);
    if(!loader.good()){
        std::cout << "-1\n";
        return 0;
    }

    clear();

    auto num = [](std::string n) -> bool {
        bool ok = !n.empty();
        for(char i : n) ok &= std::isdigit(i);
        return ok;
    };

    unsigned IDcount = 0;
    auto newID = [&](){
        return std::to_string(IDcount++) + "_AUTOGEN_ID";
    };
    
    std::vector<std::vector<std::vector<std::string> > > all;
    all.push_back({});

    std::vector<std::vector<std::string> > layers;
    std::vector<std::tuple<std::string, std::string, unsigned> > links;

    std::string s;
    while(loader >> s){

        if(s == "V"){ // complex vector definition

            std::vector<std::string> vector(3); // in, out, size
            for(auto &i : vector) loader >> i;

            if(!num(vector[2])){
                std::cout << "0\n";
                return 0;
            }

            vector.push_back("vector");
            all.back().push_back(vector);
        }
        else if(s == "L"){ // complex layer definition
            
            std::vector<std::string> layer(1); // type, arg0, arg1, ..., argn, .name
            loader >> layer[0];
            
            std::string arg;
            while(loader >> arg){
                if(arg[0] == '.'){
                    arg = arg.substr(1, arg.size()-1);
                    if(arg.empty()) arg = newID();
                    layer.push_back(arg);
                    break;
                } else {
                    layer.push_back(arg);
                }
            }

            layer.push_back("layer");
            all.back().push_back(layer);
        }
        else if(s == "J"){ // judge definition

            std::vector<std::string> judge(1); // type, arg0, arg1, ..., argn, .name
            loader >> judge[0];
            
            std::string arg;
            while(loader >> arg){
                if(arg[0] == '.'){
                    arg = arg.substr(1, arg.size()-1);
                    if(arg.empty()) arg = newID();
                    judge.push_back(arg);
                    break;
                } else {
                    judge.push_back(arg);
                }
            }

            judge.push_back("judge");
            all.back().push_back(judge);
        }
        else if(s == "{"){ // begin copy block
            all.push_back({});
        }
        else if(s == "}"){ // end copy block
            
            if(all.size() == 1){
                std::cout << "1\n";
                return 0;
            }

            loader >> s;
            if(!num(s)){
                std::cout << "2\n";
                return 0;
            }

            unsigned copies = std::stoul(s);

            while(copies--){
                for(auto i : all.back()){
                    if(i.back() == "layer"){
                        i[i.size()-2] += "_BLOCK" + std::to_string(copies);
                    }
                    else if(i.back() == "vector"){
                        for(int j : {0, 1}){
                            if(i[j] != "."){
                                if(i[j].back() == '^'){
                                    i[j].pop_back();
                                    if(i[j].empty()) i[j] = ".";
                                }
                                else i[j] += "_BLOCK" + std::to_string(copies);
                            }
                        }
                    }
                    all[all.size()-2].push_back(i);
                }
            }
            all.pop_back();
        }
        else if(num(s)){ // simple vector definition
            all.back().push_back({".", ".", s, "vector"});
        }
        else { // simpler layer definition
            all.back().push_back({s, newID(), "layer"});
        }
    }

    if(all.size() != 1){
        std::cout << "3\n";
        return 0;
    }

    std::vector<std::vector<std::string> > &raw = all[0];

    for(int i=0; i<(int)raw.size(); i++){
        if(raw[i].back() == "vector"){
            if(raw[i][0] == "."){
                bool ok = 0;
                for(int j=i-1; j>=0; j--){
                    if(raw[j].back() != "vector"){
                        raw[i][0] = raw[j][raw[j].size()-2];
                        ok = 1;
                        break;
                    }
                }
                if(!ok){
                    std::cout << "4\n";
                    return 0;
                }
            }
            if(raw[i][1] == "."){
                bool ok = 0;
                for(int j=i+1; j<(int)raw.size(); j++){
                    if(raw[j].back() != "vector"){
                        raw[i][1] = raw[j][raw[j].size()-2];
                        ok = 1;
                        break;
                    }
                }
                if(!ok){
                    std::cout << "5\n";
                    return 0;
                }
            }

            try {
                links.push_back({raw[i][0], raw[i][1], std::stoul(raw[i][2])});
            }
            catch (const std::invalid_argument &e){
                std::cout << "6\n";
                return 0;
            }
        }
        else {
            layers.push_back(raw[i]);
        }
    }

    construct(layers, links);

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
    
    lock.unlock();
}

void Stack::train_program(TrainingData &data, unsigned batchSize, unsigned batches,
        float speed, float decay){

    for(unsigned i=0; i<batches; i++){
        for(unsigned j=0; j<batchSize; j++){
            auto [input, label] = data.get_random();
            train(input, label);
        }
        apply_changes(speed, decay, batchSize);
    }
}

Stack::TestAnalysis Stack::test(TrainingData &data){

    TestAnalysis result;
    result.errors.resize(vectors.back().size(), 0.0f);

    for(size_t iter=0; iter<data.get_size(); iter++){
        
        auto i = data.get_next();

        if(!good() || i.label.size() != vectors.back().size()
                || i.input.size() != vectors[0].size()){
            return result;
        }

        run(i.input);

        unsigned best = 0, right = 0;
        for(unsigned j=0; j<i.label.size(); j++){
            if(vectors.back()[j] > vectors.back()[best]) best = j;
            if(i.label[j] > i.label[right]) right = j;
        }
        if(best == right) result.correct += 1;

        judge->score(i.label);

        for(unsigned j=0; j<i.label.size(); j++){
            result.errors[j] += std::abs(vectors.back()[j]);
        }
    }

    for(double &i : result.errors){
        i /= data.get_size();
        result.error += i;
    }

    return result;
}

void Stack::apply_changes(float speed, float decay, unsigned batchSize){
    
    float sum = 0;
    unsigned variables = 0;

    for(Layer *layer : layers){
        auto [s, v] = layer->sum_change();
        sum += s;
        variables += v;
    }

    if(variables == 0) variables = 1;

    // average root mean square. Very averageous.
    sum = std::sqrt(sum / variables) / batchSize;
    if(sum == 0.0f) sum = 1.0f;

    for(Layer *layer : layers) layer->change(speed/sum, decay);
}

void Stack::save(ui::Saver &saver){
    
    if(!good()){
        saver.write_string("BAD");
        return;
    }

    saver.write_string("GOOD");
    
    saver.write_unsigned(build_links.size());
    for(auto &i : build_links){
        auto [in, out, size] = i;
        saver.write_string(in);
        saver.write_string(out);
        saver.write_unsigned(size);
    }

    saver.write_unsigned(build_trans.size());
    for(auto &i : build_trans){
        saver.write_unsigned(i.size());
        for(auto &j : i) saver.write_string(j);
    }

    for(auto i : layers) i->save(saver);
}

void Stack::load(ui::Loader &loader){

    clear();

    if(loader.read_string() == "BAD") return;

    build_links.resize(loader.read_unsigned());
    for(auto &i : build_links){
        auto &[in, out, size] = i;
        in = loader.read_string();
        out = loader.read_string();
        size = loader.read_unsigned();
    }

    build_trans.resize(loader.read_unsigned());
    for(auto &i : build_trans){
        i.resize(loader.read_unsigned());
        for(auto &j : i) j = loader.read_string();
    }

    construct(build_trans, build_links);

    for(auto i : layers) i->load(loader);

}

void Stack::clear(){
    
    vectors.clear();

    build_links.clear();
    build_trans.clear();
    
    for(Layer *layer : layers) if(layer != nullptr) delete layer;
    layers.clear();

    if(judge != nullptr) delete judge;
    judge = nullptr;
}

}

