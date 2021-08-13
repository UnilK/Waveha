#include "ui/core.h"

#include <thread>
#include <chrono>

namespace ui{

Core::Core(){}

bool Core::loop(){

    while(windows.size()){
        for(Window *window : windows){
            window->listen_events();
            window->coreapp_update();
            window->refresh();
        }
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    return 1;
}

bool Core::add_window(Window *window){
    windows.push_back(window);
    return 1;
}

bool Core::delete_window(Window *window){
    windows.remove(window);
    return 1;
}

}

