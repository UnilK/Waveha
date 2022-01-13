#pragma once

#include "ui/text.h"

namespace ui {

class Button : public ui::Text {

    /*
       style:
       normal (look)
       pressed (look)

       kwargs:
       -
     */

public:

    Button(Window *master_, void *commander_, kwargs values = {});
    virtual ~Button();

    int32_t set_look(std::string look_);
    
    int32_t on_event(sf::Event event, int32_t priority);

protected:

    void *commander = nullptr;

    virtual void function() = 0;

    bool buttonPressed = 0;

};

}

