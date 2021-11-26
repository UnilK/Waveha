#include "app/app.h"

#include <iostream>

App::App() :
    Core(1, 1, "res/styles/default.style", 1000),
    window(this)
{}

int32_t App::execute_command(std::string command){
    std::cout << command << '\n';
    return 0;
}

