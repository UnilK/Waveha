#pragma once

namespace ui{ class Window; }

#include "ui/core.h"
#include "ui/frame.h"
#include "ui/clock.h"

#include <cstdint>
#include <string>
#include <set>
#include <map>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace ui{

class Window : public sf::RenderWindow {
 
    // Class built around sf::RenderWindow.

public:
   
    // mouse position
    float mouseX = 0, mouseY = 0;

    Window(Core *core_, float width_, float height_,
            std::string title_, long double refreshRate = 60);
   
    Clock clock;

    // delete the window and set the destroyed flag.
    int32_t destroy();
    bool destroyed = 0;

    // event updates.
    int32_t event_update();
    
    // actions run before every refresh.
    int32_t try_tick();
    int32_t tick();
    virtual int32_t on_tick();

    // Redraw & render.
    int32_t refresh();

    // inform the window that it should refresh itself
    void set_refresh();

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

    void update_style();

protected:

    Core *core;
    Frame *mainframe;
    std::set<Window*> children;

    std::string title;

    float height = 100, width = 100;
    sf::Texture previousFrame;
    sf::Sprite previousFrameSprite;
    bool resizeFlag = 1, refreshFlag = 1, eventTick = 0;

    Frame *softFocus = nullptr;
    Frame *hardFocus = nullptr;

};

}

