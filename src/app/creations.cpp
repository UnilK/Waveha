#include "app/creations.h"
#include "app/app.h"
#include "app/session.h"
#include "ml/mnist.h"
#include "ml/stack.h"

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
        saver.write_string(std::get<0>(i.second));
        saver.write_string(std::get<1>(i.second));
        saver.write_string(i.first);
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
    for(unsigned i=0; i<size; i++)
        load_mldata(loader.read_string(), loader.read_string(), loader.read_string());

    size = loader.read_unsigned();
    for(unsigned i=0; i<size; i++){
        std::string name = loader.read_string();
        stacks[name] = new ml::Stack();
        stacks[name]->load(loader);
    }
}

void Creations::reset(){
    for(auto &i : datas) delete std::get<2>(i.second);
    for(auto &i : stacks) delete i.second;
    datas.clear();
    stacks.clear();
}

int Creations::load_mldata(std::string type, std::string file, std::string name){

    if(type == "mnist"){
        ml::TrainingData *data = ml::mnist_data(file);
        if(data == nullptr) return 1;
        remove_mldata(name);
        datas[name] = {type, file, data};
    }
    else {
        return 1;
    }

    return 0;
}

int Creations::remove_mldata(std::string name){
    
    if(datas.count(name)) return 1;
    delete std::get<2>(datas[name]);
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
    
    if(stacks.count(name)) return 1;
    delete stacks[name];
    datas.erase(name);

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

// directory //////////////////////////////////////////////////////////////////

CreationsDir::CreationsDir(Creations &c) : creations(c) {

    put_function("list", [&](ui::Command c){ list_stuff(c); });
    put_function("dload", [&](ui::Command c){ load_mldata(c); });
    put_function("ddel", [&](ui::Command c){ remove_mldata(c); });
    put_function("screate", [&](ui::Command c){ create_stack(c); });
    put_function("sload", [&](ui::Command c){ load_stack(c); });
    put_function("ssave", [&](ui::Command c){ save_stack(c); });
    put_function("sdel", [&](ui::Command c){ remove_stack(c); });

    document("list", "list creations");
    document("dload", "[type] [file] {name} load data for ml stacks");
    document("ddel", "[name] delete ml data");
    document("screate", "[file] {name} create a ml stack from a file");
    document("sload", "[file] {name} load a ml stack");
    document("ssave", "[name] {file} save a ml stack");
    document("sdel", "[name] delete a ml stack");
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
