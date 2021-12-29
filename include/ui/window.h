#pragma once

namespace ui{ class Window; }

#include "ui/clock.h"
#include "ui/core.h"
#include "ui/frame.h"
#include "ui/command.h"

#include <cstdint>
#include <string>
#include <set>
#include <map>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>

namespace ui{

class Window : public sf::RenderWindow, public Commandable {
 
    // Class built around sf::RenderWindow.

public:
   
    // mouse position
    float mouseX = 0, mouseY = 0;

    Clock clock;

    Window(Core *core_, float width_, float height_,
            std::string title_, long double refreshRate = 60);
    
    // delete the window and set the destroyed flag.
    int32_t destroy();
    bool destroyed = 0;

    // set this flag if content in window changes
    bool displayFlag = 0;

    // event updates.
    int32_t event_update();
    
    // ticks
    int32_t core_tick();
    int32_t window_tick();

    // extra actions related to ticks. 
    virtual int32_t on_core_tick();
    virtual int32_t on_window_tick();

    // Redraw & render.
    int32_t refresh();

    // attach & detach child windows
    int32_t attach_child(Window *child);
    int32_t detach_child(Window *child);

    std::vector<Frame*> focus;
    Frame *get_soft_focus();
    Frame *get_hard_focus();
    Core *get_core();

    // overloadable responses to special events.
    virtual int32_t on_close();
    virtual int32_t on_resize();

protected:

    Core *core;
    Frame *mainframe;
    std::set<Window*> children;

    std::string title;

    float height = 100, width = 100;

    Frame *softFocus = nullptr;
    Frame *hardFocus = nullptr;

};

}

