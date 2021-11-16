#pragma once

namespace ui{
class Window;
}

#include "ui/core.h"
#include "ui/frame.h"

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include <SFML/Graphics/RenderWindow.hpp>

namespace ui{

class Window : public sf::RenderWindow {
 
    // Class built around sf::RenderWindow.

    protected:

        Core *core;
        Frame *mainframe;

        std::string title;

        float height = 100, width = 100;

        Frame *soft_focus = nullptr;
        Frame *hard_focus = nullptr;
        Frame *clicked = nullptr;

    public:
       
        // mouse position
        float mouseX = 0, mouseY = 0;

        Window(Core *core_, float width_, float height_, std::string title_);
        
        // delete the window and set the destroyed flag.
        int32_t destroy();
        bool destroyed = 0;

        // set this flag if content in window changes
        bool displayFlag = 0;

        // updates.
        int32_t coreapp_update();
        int32_t listen_events();

        // Redraw & render.
        int32_t refresh();

        Frame *get_soft_focus();
        Frame *get_hard_focus();
        Frame *get_clicked();
        Core *get_core();

        // overloadable responses to events.
        virtual int32_t on_close();
        virtual int32_t on_resize();
        virtual int32_t on_mouse_move();

    };

}

