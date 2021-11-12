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

Core::Core(bool hasTerminal_, bool hasWindow_){
    hasTerminal = hasTerminal_;
    hasWindow = hasWindow_;
    if(hasTerminal){
        terminal = new std::thread(listen_terminal, this);
        terminal->detach();
    }
}

int32_t Core::start(){

    running = 1;

    while(running){

        if(hasTerminal){
            command_lock.lock();
            for(std::string &command : commands) execute_command(command);
            commands.clear();
            command_lock.unlock();
        }

        if(hasWindow){
            for(Window *window : windows){
                window->listen_events();
                window->coreapp_update();
                window->refresh();
            }
            clean_windows();
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
    if(windows.count(window)) return 1;
    windows.insert(window);
    return 0;
}

int32_t Core::clean_windows(){
    
    std::set<Window*> cleaned;
    for(auto w : windows) if(!w->destroyed) cleaned.insert(w);
    windows = cleaned;

    return 0;
}

int32_t Core::add_command(std::string command){
    command_lock.lock();
    commands.push_back(command);
    command_lock.unlock();
    return 0;
}

}

