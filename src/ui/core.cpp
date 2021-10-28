#include "ui/core.h"

#include <chrono>
#include <iostream>

namespace ui{

void listen_terminal(Core *core){
    std::string command = "";
    while(1){
        std::string input;
        std::getline(std::cin, input);
        command += input;
        if(command.back() != '\\'){
            core->add_command(command);
            command.clear();
        } else command.pop_back();
    }
}

Core::Core(){
    terminal = std::thread(listen_terminal, this);
}

int32_t Core::start(){

    while(running){
        
        for(std::string &command : commands) execute_command(command);
        commands.clear();

        for(Window *window : windows){
            window->listen_events();
            window->coreapp_update();
            window->refresh();
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }

    return 0;
}

int32_t Core::stop(){
    running = 0;
    return 0;
}

int32_t Core::add_window(Window *window){
    windows.push_back(window);
    return 0;
}

int32_t Core::delete_window(Window *window){
    windows.remove(window);
    delete window;
    return 0;
}

int32_t Core::add_command(std::string command){
    commands.push_back(command);
    return 0;
}

}

