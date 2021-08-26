#ifndef UI_CORE_H
#define UI_CORE_H

namespace ui{
class Core;
}

#include "ui/window.h"
#include "ui/style.h"

#include <list>
#include <vector>
#include <thread>

namespace ui{

class Core;
class Window;

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

    bool start();
    bool stop();
    
    bool add_window(Window *window);
    bool delete_window(Window *window);

    bool add_command(std::string command);
    bool execute_command(std::string command);
};

}

#endif
