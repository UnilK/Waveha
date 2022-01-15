#include "ui/core.h"

#include <chrono>
#include <iostream>
#include <fstream>

namespace ui {

Core *Core::object = nullptr;

void listen_terminal(Core *core){
    std::string command = "";
    while(!std::cin.eof()){
        std::string input;
        std::getline(std::cin, input);
        command += input;
        if(command.back() != '\\'){
            core->command_from_terminal(command);
            command.clear();
        } else command.pop_back();
    }
    std::cout << "terminal input polling terminated\n";
}

Core::Core(std::string styleFile, long double tickRate) :
    clock(1.0l/tickRate)
{
    Style::load(styleFile);
    commandId = "";
    commandRoot = this;
    commandFocus = this;
    terminal = new std::thread(listen_terminal, this);
    terminal->detach();
    object = this;
}

int32_t Core::start(){

    running = 1;

    while(running){

        terminalCommandLock.lock();

        for(auto cmd : terminalCommands){
            Command command = commandFocus->create_command(cmd);
            commandFocus->deliver_address(command);
        }

        terminalCommands.clear();
        
        terminalCommandLock.unlock();

        for(Window *window : windows){
            window->event_update();
            window->tick();
        }
        clean_windows();

        clock.join_tick();
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

int32_t Core::command_from_terminal(std::string command){
    terminalCommandLock.lock();
    terminalCommands.push_back(command);
    terminalCommandLock.unlock();
    return 0;
}

int32_t Core::set_style(std::string styleFile){

    std::ifstream I(styleFile);

    if(I.good()){
        Style::load(styleFile);
        update_style();
        return 0;
    } else {
        return 1;
    }
}

void Core::update_style(){
    for(auto w : windows) if(!w->destroyed) w->update_style();    
}

}

