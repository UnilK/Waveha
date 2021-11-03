#pragma once

namespace ui{
class Core;
}

#include "ui/window.h"
#include "ui/style.h"

#include <set>
#include <vector>
#include <thread>
#include <mutex>

namespace ui{

class Core{
    
    /*
       Manages the main frame window(s), main loop and terminal.
       Template class for applications.
    */

protected:

    std::set<Window*> windows;

    std::thread *terminal;
    std::mutex command_lock;
    std::vector<std::string> commands;

    bool hasTerminal;
    bool hasWindow;
    bool running = 0;

public:

    Style style;

    Core(bool hasTerminal_, bool hasWindow_);

    int32_t start();
    int32_t stop();
    
    int32_t add_window(Window *window);
    int32_t clean_windows();

    int32_t add_command(std::string command);
    virtual int32_t execute_command(std::string command) = 0;
};

}

