#include "ui/button.h"

#include <iostream>
#include <sstream>
#include <math.h>
#include <string>

namespace ui {
    
Button::Button(Window *master_, std::function<void(void)> func, std::string text_, Kwargs kwargs) :
    Text(master_, text_, kwargs),
    function(func)
{
    set_look(look);
}

Button::~Button(){}

void Button::set_look(std::string look_){
    
    look = look_;

    if(buttonPressed) Text::set_look(chars("pressed"));
    else Text::set_look(chars("normal"));

    look = look_;
}

Frame::Capture Button::on_event(sf::Event event, int32_t priority){
   
    if(priority > 0) return Capture::pass;

    if(event.type == sf::Event::MouseButtonPressed){
        buttonPressed = 1;
        set_refresh();
        set_look(look);
        return Capture::capture;
    }
    else if(event.type == sf::Event::MouseButtonReleased){
        buttonPressed = 0;
        set_refresh();
        set_look(look);
        if(master->get_soft_focus() == this) function();
        return Capture::capture;
    }

    return Capture::pass;
}

}

