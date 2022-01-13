#include "ui/button.h"

#include <iostream>
#include <sstream>
#include <math.h>
#include <string>

namespace ui {
    
Button::Button(Window *master_, void *commander_, kwargs values) :
    Text(master_, values),
    commander(commander_)
{
    set_look(look);
}

Button::~Button(){}

int32_t Button::set_look(std::string look_){
    
    look = look_;

    if(buttonPressed) look = chars("pressed");
    else look = chars("normal");

    textBox.setString(text);
    textBox.setFont(font("font"));
    textBox.setStyle(textStyle("textStyle"));
    textBox.setCharacterSize(num("textSize"));
    textBox.setFillColor(color("textColor"));

    border.set_look(look);

    look = look_;

    inner_reconfig();

    return 0;
}

int32_t Button::on_event(sf::Event event, int32_t priority){
   
    if(priority > 0) return -1;

    if(event.type == sf::Event::MouseButtonPressed){
        buttonPressed = 1;
        set_refresh();
        set_look(look);
    }
    else if(event.type == sf::Event::MouseButtonReleased){
        buttonPressed = 0;
        set_refresh();
        set_look(look);
        if(master->get_soft_focus() == this) function();
    }

    return 0;
}

}

