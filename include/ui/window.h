#pragma once

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

namespace ui {

class Window : public sf::RenderWindow, public Frame {

public:
   
    Window(float width, float height, std::string title);
    virtual ~Window();
   
    // delete the window and set the destroyed flag.
    void destroy();

    // event updates.
    void event_update();

    // begins the event loop. only one frame can call this per thread.
    void loop();

    // redraw mainFrame
    void window_refresh();

    // mouse position
    std::array<float, 2> mouse();

    // child window management
    void attach_child(Window *child);
    void kill_child(Window *child);
    void kill_children();

    void style();

    void reset_hard_focus();
    std::vector<Frame*> focus;
    Frame *get_soft_focus();
    Frame *get_hard_focus();

    // overloadable responses to special events.
    virtual void on_close();
    virtual void on_resize();

private:

    std::set<Window*> children;

    sf::Texture previousFrame;
    sf::Sprite previousFrameSprite;
    bool resizeFlag = 1;
   
    void set_hard();

    float mouseX = 0, mouseY = 0;
    bool destroyed = 0;

    Frame *softFocus = nullptr;
    Frame *hardFocus = nullptr;

};

}

