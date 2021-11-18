#include "ui/button.h"

#include <iostream>
#include <sstream>

namespace ui{
    
Button::Button(Frame *parent_, int32_t(*function_)(void *),
            void *target_, std::map<std::string, std::string> values) :
    ContentFrame(parent_, values),
    function(function_),
    target(target_)
{
    setup(values);
}

int32_t Button::setup(std::map<std::string, std::string> &values){
    
    std::stringstream value;
    
    if(read_value("text", value, values)){
        text = "";
        std::string word;
        while(std::getline(value, word)) text += word+"\n";
        if(!text.empty()) text.pop_back();
    }
    if(read_value("border", value, values))
        value >> borderLeft >> borderRight >> borderUp >> borderDown;
    
    return 0;
}

int32_t Button::draw(){

    initialize();
   
    Look l = core->style.get_look(look);

    if(pressed) canvas.clear(l.color("pressed_background"));
    else canvas.clear(l.color("background"));

    sf::RectangleShape left({borderLeft, canvasHeight});
    sf::RectangleShape right({borderRight, canvasHeight});
    sf::RectangleShape up({canvasWidth, borderUp});
    sf::RectangleShape down({canvasWidth, borderDown});

    sf::Text textBox(text, l.font("font"), l.num("text_size"));
    textBox.setFillColor(l.color("text_color"));
    
    sf::FloatRect rect = textBox.getGlobalBounds();
    
    float textX = (canvasWidth - borderRight + borderLeft) / 2 - rect.width / 2;
    float textY = (canvasHeight - borderDown + borderUp) / 2 - rect.height;

    textBox.setPosition(textX, textY);

    left.setPosition(0, 0);
    right.setPosition(canvasWidth - borderRight, 0);
    up.setPosition(0, 0);
    down.setPosition(0, canvasHeight - borderDown);

    left.setFillColor(l.color("border"));
    right.setFillColor(l.color("border"));
    up.setFillColor(l.color("border"));
    down.setFillColor(l.color("border"));

    canvas.draw(left);
    canvas.draw(right);
    canvas.draw(up);
    canvas.draw(down);
    canvas.draw(textBox);

    display();

    return 0;
}

int32_t Button::event_update(sf::Event event){
   
    if(event.type == sf::Event::MouseButtonPressed){
        pressed = 1;
        refreshFlag = 1;
    }
    else if(event.type == sf::Event::MouseButtonReleased){
        pressed = 0;
        refreshFlag = 1;
        if(master->get_clicked() == this && master->get_soft_focus() == this)
            function(target);
    }

    return 0;
}

}

