#pragma once

namespace ui { class Core; }

#include "ui/window.h"
#include "ui/style.h"
#include "ui/clock.h"
#include "ui/command.h"

#include <set>
#include <vector>
#include <thread>
#include <mutex>

namespace ui {

class Core : public Commandable {
    
    /*
       Manages the main frame window(s), main loop and terminal.
       Template class for applications.
    */

public:

    // singleton
    static Core *object;
    
    Style style;
    Clock clock;

    Core(std::string styleFile, long double tickRate = 1000);

    int32_t start();
    int32_t stop();
    
    int32_t add_window(Window *window);
    int32_t clean_windows();

    int32_t command_from_terminal(std::string command);
    
    int32_t set_style(std::string styleFile);
    void update_style();

protected:

    std::set<Window*> windows;

    std::thread *terminal;
    std::mutex terminalCommandLock;

    std::vector<std::string> terminalCommands;

    bool running = 0;

};

}

