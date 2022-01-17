#pragma once

#include "ui/text.h"

#include <functional>

namespace ui {

class Button : public ui::Text {

    /*
       style:
       normal (ui::text look)
       pressed (ui::text look)
     */

public:

    Button(Window *master_, std::function<void(void)> func, std::string text_ = "", Kwargs = {});
    virtual ~Button();

    void set_look(std::string look_);
    
    Capture on_event(sf::Event event, int32_t priority);

private:

    std::function<void(void)> function;

    bool buttonPressed = 0;

};

}

