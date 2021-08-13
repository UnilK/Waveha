#ifndef UI_CORE_H
#define UI_CORE_H

namespace ui{
class Core;
}

#include "ui/style.h"
#include "ui/window.h"

#include <list>

namespace ui{

class Core{
    
    /*
        Manages the main frame window(s).
    */

protected:

    std::list<Window*> windows;

public:

    Style style;

    Core();

    bool loop();
    
    bool add_window(Window *window);
    bool delete_window(Window *window);
    
};

}

#endif
