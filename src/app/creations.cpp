#include "app/creations.h"

namespace app {

// creations //////////////////////////////////////////////////////////////////

Creations::Creations(App *a) :
    dir(*this),
    app(*a)
{}

int Creations::load_data(std::string type, std::string file){
    return 0;
}

int Creations::remove_data(std::string name){
    return 0;
}

int Creations::load_stack(std::string name, std::string file){
    return 0;
}

int Creations::save_stack(std::string name, std::string file){
    return 0;
}

int Creations::remove_stack(std::string name){
    return 0;
}

// directory //////////////////////////////////////////////////////////////////

CreationsDir::CreationsDir(Creations &c) : creations(c) {

    put_function("list", [&](ui::Command c){ list_stuff(c); });
    put_function("dload", [&](ui::Command c){ load_data(c); });
    put_function("dsave", [&](ui::Command c){ remove_data(c); });
    put_function("sload", [&](ui::Command c){ load_stack(c); });
    put_function("ssave", [&](ui::Command c){ remove_stack(c); });
    put_function("sdel", [&](ui::Command c){ remove_stack(c); });

    document("list", "list creations");
    document("dload", "[file] {name} load data for ml stacks");
    document("ddel", "[name] delete ml data");
    document("sload", "[file] {name} load a ml stack");
    document("ssave", "[name] {file} save a ml stack");
    document("sdel", "[name] delete a ml stack");
}

void CreationsDir::load_data(ui::Command c){

}

void CreationsDir::remove_data(ui::Command c){

}

void CreationsDir::list_stuff(ui::Command c){

}

void CreationsDir::load_stack(ui::Command c){

}

void CreationsDir::save_stack(ui::Command c){

}

void CreationsDir::remove_stack(ui::Command c){

}


}
