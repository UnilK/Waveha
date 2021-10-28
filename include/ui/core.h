#pragma once

namespace ui{
class Core;
}

#include "ui/window.h"
#include "ui/style.h"

#include <list>
#include <vector>
#include <thread>

namespace ui{

void listen_terminal(Core *core);

class Core{
    
    /*
       Manages the main frame window(s), main loop and terminal.
       Template class for applications.
    */

protected:

    std::list<Window*> windows;
    std::vector<std::string> commands;
    std::thread terminal;

    bool running = 0;

public:

    Style style;

    Core();

    int32_t start();
    int32_t stop();
    
    int32_t add_window(Window *window);
    int32_t delete_window(Window *window);

    int32_t add_command(std::string command);
    virtual int32_t execute_command(std::string command) = 0;
};

}

