#pragma once

#include "ui/text.h"

namespace ui{

class Button : public ui::Text {

    /*
       my design pattern need improvement here.

       create a button on a parent frame.
       give the parent frame static method that takes
       it's own pointer as input.
       pass static function and "this" to button.

       maybe lambdas? template magic?



       style:
       normal (look)
       pressed (look)

       kwargs:
       -
     */

public:

    Button(Frame *parent_, int32_t (*function_)(void*), void *target_, kwargs values = {});

    int32_t set_look(std::string look_);
    
    int32_t on_event(sf::Event event, int32_t priority);

protected:

    std::string baseLook;

    bool buttonPressed = 0;
    int32_t (*function)(void*);
    void *target;

};

}

