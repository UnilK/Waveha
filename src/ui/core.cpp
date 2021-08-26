#include "ui/core.h"

#include <chrono>
#include <iostream>

namespace ui{

void listen_terminal(Core *core){
    std::string command = "";
    while(1){
        std::string input;
        std::cin >> input;
        command += input;
        if(command.back() != '\\'){
            core->add_command(command);
            command.clear();
        }
    }
}

Core::Core(){
    terminal = std::thread(listen_terminal, this);
}

bool Core::start(){

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

    return 1;
}

bool Core::stop(){
    running = 0;
    return 1;
}

bool Core::add_window(Window *window){
    windows.push_back(window);
    return 1;
}

bool Core::delete_window(Window *window){
    windows.remove(window);
    delete window;
    return 1;
}

bool Core::add_command(std::string command){
    commands.push_back(command);
    return 1;
}

bool Core::execute_command(std::string command){
    // app should override this
    return 1;
}

}

