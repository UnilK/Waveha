#pragma once

#include "ui/text.h"

#include <functional>

namespace ui {

class Button : public ui::Text {

    /*
       style:
       normal (ui::Text look)
       pressed (ui::Text look)
     */

public:

    Button(Window *master_, std::function<void(void)> func, std::string text_ = "", Kwargs = {});
    virtual ~Button();

    void set_look(std::string look_);
    
    Capture on_event(sf::Event event, int priority);

private:

    std::function<void(void)> function;

    bool buttonPressed = 0;

};

}

