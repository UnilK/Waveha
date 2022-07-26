#include "app/creations.h"
#include "app/app.h"
#include "app/session.h"
#include "ml/mnist.h"
#include "ml/stack.h"
#include "ml/waves.h"
#include "ml/db.h"

#include <iostream>

namespace app {

// creations //////////////////////////////////////////////////////////////////

Creations::Creations(App *a) :
    dir(*this),
    app(*a)
{}

Creations::~Creations(){
    reset();
}

void Creations::save(ui::Saver &saver){
    
    saver.write_unsigned(datas.size());
    for(auto &i : datas){
        auto [type, file, pointer] = i.second;
        saver.write_string(i.first);
        saver.write_string(type);
        saver.write_string(file);
    }

    saver.write_unsigned(stacks.size());
    for(auto &i : stacks){
        saver.write_string(i.first);
        i.second->save(saver);
    }

}

void Creations::load(ui::Loader &loader){
    
    reset();
    
    unsigned size = loader.read_unsigned();
    for(unsigned i=0; i<size; i++){
        auto name = loader.read_string();
        auto type = loader.read_string();
        auto file = loader.read_string();
        load_mldata(type, file, name);
    }

    size = loader.read_unsigned();
    for(unsigned i=0; i<size; i++){
        std::string name = loader.read_string();
        stacks[name] = new ml::Stack();
        stacks[name]->load(loader);
    }
}

void Creations::reset(){

    std::vector<std::string> tdatas, tstacks;

    for(auto [k, v] : datas) tdatas.push_back(k);
    for(auto [k, v] : stacks) tstacks.push_back(k);
    
    for(auto i : tdatas) remove_mldata(i);
    for(auto i : tstacks) remove_stack(i);

    datas.clear();
    stacks.clear();
}

int Creations::create_waves(std::string directory, std::string output, unsigned density){
    return ml::create_wave_data(directory, output, density);
}

int Creations::blur_mnist(std::string name){

    if(!datas.count(name)) return 1;
    
    auto &[type, file, pointer] = datas[name];
    
    if(type != "mnist") return 2;

    ((ml::MnistData*)pointer)->blur();
    
    return 0;
}

int Creations::ft_mnist(std::string name){

    // NOP
    
    return 0;
}

int Creations::load_mldata(std::string type, std::string file, std::string name){

    if(type == "mnist"){
        
        ml::MnistData *data = ml::mnist_data(file);
        
        if(data == nullptr) return 1;
        remove_mldata(name);
        datas[name] = {type, file, data};
        
    }
    else if(type == "wave"){

        ml::WaveData *data = ml::wave_data(file);
        if(data == nullptr) return 1;

        remove_mldata(name);

        datas[name] = {type, file, data};
        datas[name+"r"] = {type+"r", "", &data->raw};
        datas[name+"l"] = {type+"l", "", &data->labeled};
        datas[name+"m"] = {type+"m", "", &data->matched};
    }
    else {
        return 1;
    }

    return 0;
}

int Creations::remove_mldata(std::string name){
    
    if(!datas.count(name)) return 1;

    auto [type, file, pointer] = datas[name];
  
    if(type == "waver") return 2;
    if(type == "wavel") return 2;
    if(type == "wavem") return 2;

    if(type == "wave"){
        datas.erase(name+"r");
        datas.erase(name+"l");
        datas.erase(name+"m");
    }

    delete pointer;
    datas.erase(name);
    
    return 0;
}

int Creations::create_stack(std::string name, std::string file){
    
    remove_stack(name);
    stacks[name] = new ml::Stack();
    stacks[name]->construct_from_file(file);

    return !stacks[name]->good();
}

int Creations::load_stack(std::string name, std::string file){
    
    remove_stack(name);
    stacks[name] = new ml::Stack();

    ui::Loader loader(file);
    stacks[name]->load(loader);

    return 0;
}

int Creations::save_stack(std::string name, std::string file){
    
    if(stacks.count(name) == 0) return 1;

    ui::Saver saver(file);
    stacks[name]->save(saver);
    
    return 0;
}

int Creations::remove_stack(std::string name){
    
    if(!stacks.count(name)) return 1;
    delete stacks[name];
    stacks.erase(name);

    return 0;
}

ml::TrainingData *Creations::get_mldata(std::string name){
    if(datas.count(name)) return std::get<2>(datas[name]);
    return nullptr;
}

ml::Stack *Creations::get_stack(std::string name){
    if(stacks.count(name)) return stacks[name];
    return nullptr;
}

std::string Creations::data_type(std::string name){
    if(datas.count(name)) return std::get<0>(datas[name]);
    return "";
}

// directory //////////////////////////////////////////////////////////////////

CreationsDir::CreationsDir(Creations &c) : creations(c) {

    put_function("list", [&](ui::Command c){ list_stuff(c); });
    put_function("dload", [&](ui::Command c){ load_mldata(c); });
    put_function("ddel", [&](ui::Command c){ remove_mldata(c); });
    put_function("wcreate", [&](ui::Command c){ create_waves(c); });
    put_function("screate", [&](ui::Command c){ create_stack(c); });
    put_function("sload", [&](ui::Command c){ load_stack(c); });
    put_function("ssave", [&](ui::Command c){ save_stack(c); });
    put_function("sdel", [&](ui::Command c){ remove_stack(c); });
    put_function("mblur", [&](ui::Command c){ blur_mnist(c); });
    put_function("mft", [&](ui::Command c){ ft_mnist(c); });

    document("list", "list creations");
    document("dload", "[type] [file] {name} load data for ml stacks");
    document("ddel", "[name] delete ml data");
    document("wcreate", "[directory] [output] [density] create wave data from directory of .wav files");
    document("screate", "[file] {name} create a ml stack from a file");
    document("sload", "[file] {name} load a ml stack");
    document("ssave", "[name] {file} save a ml stack");
    document("sdel", "[name] delete a ml stack");
    document("mblur", "[name] blur images in mnist dataset");
    document("mft", "[name] fourier transform images in mnist dataset");
}

void CreationsDir::create_waves(ui::Command c){
    
    try {
        std::string dir = c.pop();
        std::string out = c.pop();
        unsigned d = std::stoul(c.pop());

        if(dir.empty() || out.empty()){
            c.source.push_error("give a directory and the output file name");
            return;
        }

        int ret = creations.create_waves(dir, out, d);

        if(ret) c.source.push_error("error");
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void CreationsDir::blur_mnist(ui::Command c){

    std::string name = c.pop();

    if(name.empty()){
        c.source.push_error("give mnist data name");
    } else {
        int code = creations.blur_mnist(name);
        if(code == 1) c.source.push_error("data not found: " + name);
        else if(code == 2) c.source.push_error(name + " is not mnist data");
    }
}

void CreationsDir::ft_mnist(ui::Command c){

    std::string name = c.pop();

    if(name.empty()){
        c.source.push_error("give mnist data name");
    } else {
        // NOP
    }
}

void CreationsDir::load_mldata(ui::Command c){

    std::string type = c.pop(), file = c.pop(), name = c.pop();
    if(name.empty()) name = file;

    if(type.empty() || file.empty()){
        c.source.push_error("give a type and a file");
    } else {
        int code = creations.load_mldata(type, file, name);
        if(code == 1) c.source.push_error("file not found: " + file);
    }
}

void CreationsDir::remove_mldata(ui::Command c){

    std::string name = c.pop();

    if(name.empty()){
        c.source.push_error("give data name");
    } else {
        int code = creations.remove_mldata(name);
        if(code == 1) c.source.push_error("data not found: " + name);
    }
}

void CreationsDir::list_stuff(ui::Command c){
    
    std::string message;

    message += "datas:\n";
    if(creations.datas.empty()) message += "(empty)\n";
    else for(auto &i : creations.datas) message += i.first + "\n";
    
    message += "stacks:\n";
    if(creations.stacks.empty()) message += "(empty)\n";
    else for(auto &i : creations.stacks) message += i.first + "\n";
    
    message.pop_back();
    c.source.push_output(message);
}

void CreationsDir::create_stack(ui::Command c){
    
    std::string suffix = c.pop(), name = c.pop();
    std::string file = "stacks/sources/" + suffix;
    
    if(name.empty()) name = suffix;

    if(file.empty()){
        c.source.push_error("give a file");
    } else {
        if(creations.create_stack(name, file)){
            c.source.push_error("bad stack");
        }
    }
}

void CreationsDir::load_stack(ui::Command c){

    std::string suffix = c.pop(), name = c.pop();
    std::string file = "stacks/saves/" + suffix;

    if(name.empty()) name = suffix;

    if(file.empty()){
        c.source.push_error("give a file");
    } else {
        creations.load_stack(name, file);
    }
}

void CreationsDir::save_stack(ui::Command c){
    
    std::string name = c.pop(), file = c.pop();
    if(file.empty()) file = "stacks/saves/" + name;

    if(file.empty()){
        c.source.push_error("give a name");
    } else {
        int code = creations.save_stack(name, file);
        if(code == 1) c.source.push_error("stack not found: " + name);
    }

}

void CreationsDir::remove_stack(ui::Command c){
    
    std::string name = c.pop();
    
    if(name.empty()){
        c.source.push_error("give stack name");
    } else {
        int code = creations.remove_stack(name);
        if(code == 1) c.source.push_error("stack not found: " + name);
    }
}

}
