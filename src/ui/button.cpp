#include "ui/button.h"

#include <iostream>
#include <sstream>
#include <math.h>
#include <string>

namespace ui{
    
Button::Button(Frame *parent_, int32_t(*function_)(void *), void *target_, kwargs values) :
    Text(parent_, values),
    function(function_),
    target(target_)
{
    set_look(look);
}

int32_t Button::set_look(std::string look_){
    
    look = look_;

    if(pressed) look = chars("pressed");
    else look = chars("normal");

    textBox.setString(text);
    textBox.setFont(font("font"));
    textBox.setStyle(textStyle("textStyle"));
    textBox.setCharacterSize(num("textSize"));
    textBox.setFillColor(color("textColor"));

    border.set_look(this);

    look = look_;

    inner_reconfig();

    return 0;
}

int32_t Button::on_event(sf::Event event, int32_t priority){
   
    if(priority > 0) return -1;

    if(event.type == sf::Event::MouseButtonPressed){
        pressed = 1;
        set_refresh();
        set_look(look);
    }
    else if(event.type == sf::Event::MouseButtonReleased){
        pressed = 0;
        set_refresh();
        set_look(look);
        if(master->get_soft_focus() == this)
            function(target);
    }

    return 0;
}

}

