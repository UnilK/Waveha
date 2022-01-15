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

    Button(Window *master_, void *commander_, std::string text_ = "", Kwargs = {});
    virtual ~Button();

    void set_look(std::string look_);
    
    Capture on_event(sf::Event event, int32_t priority);

protected:

    void *commander = nullptr;

    virtual void function() = 0;

    bool buttonPressed = 0;

};

}

