#include "ui/button.h"

#include <iostream>
#include <sstream>
#include <math.h>
#include <string>

namespace ui {
    
Button::Button(Window *master_, void *commander_, std::string text_, Kwargs kwargs) :
    Text(master_, text_, kwargs),
    commander(commander_)
{
    set_look(look);
}

Button::~Button(){}

void Button::set_look(std::string look_){
    
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

    on_reconfig();
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

