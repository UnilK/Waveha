#include "ui/button.h"

#include <iostream>
#include <sstream>
#include <math.h>
#include <string>

namespace ui{
    
Button::Button(Frame *parent_, int32_t(*function_)(void *), void *target_, kwargs values) :
    ContentFrame(parent_, values),
    function(function_),
    target(target_)
{
    setup(values);
    
    textBox.setString(text);
    textBox.setStyle(textStyle("textStyle"));
    textBox.setFont(font("font"));
    textBox.setCharacterSize(num("textSize"));
    textBox.setFillColor(color("textColor"));
    
    left.setFillColor(color("borderColor"));
    right.setFillColor(color("borderColor"));
    up.setFillColor(color("borderColor"));
    down.setFillColor(color("borderColor"));

    std::stringstream probe(chars("borderThickness")), borders(chars("borderThickness"));
    std::string thickness;
    int32_t total = 0;

    while(probe >> thickness) total++;

    if(total == 1){
        borders >> borderDown;
        borderLeft = borderRight = borderUp = borderDown;
    } else if(total == 4){
        borders >> borderLeft >> borderRight >> borderUp >> borderDown;
    }

    canvas.setSmooth(0);
}

int32_t Button::setup(kwargs &values){
    
    std::stringstream value;
    
    if(read_value("text", value, values)){
        text = "";
        std::string word;
        while(std::getline(value, word)) text += word+"\n";
        if(!text.empty()) text.pop_back();
    }
    
    return 0;
}

int32_t Button::inner_reconfig(){
    
    left.setSize({borderLeft, canvasHeight});
    right.setSize({borderRight, canvasHeight});
    up.setSize({canvasWidth, borderUp});
    down.setSize({canvasWidth, borderDown});
    
    sf::FloatRect rect = textBox.getLocalBounds();
    
    float textX = std::round((canvasWidth - borderRight + borderLeft) / 2 - rect.width / 2);
    float textY = std::round((canvasHeight - borderDown + borderUp) / 2
            - textBox.getCharacterSize() * 1.35f / 2);

    textBox.setPosition(textX, textY);
    
    left.setPosition(0, 0);
    right.setPosition(canvasWidth - borderRight, 0);
    up.setPosition(0, 0);
    down.setPosition(0, canvasHeight - borderDown);


    return 0;
}

int32_t Button::draw(){

    if(pressed) canvas.clear(color("pressedBackground"));
    else canvas.clear(color("background"));

    canvas.draw(left);
    canvas.draw(right);
    canvas.draw(up);
    canvas.draw(down);
    canvas.draw(textBox);

    display();

    return 0;
}

int32_t Button::on_event(sf::Event event, int32_t priority){
   
    if(priority > 0) return -1;

    if(event.type == sf::Event::MouseButtonPressed){
        pressed = 1;
        set_refresh();
    }
    else if(event.type == sf::Event::MouseButtonReleased){
        pressed = 0;
        set_refresh();
        if(master->get_soft_focus() == this)
            function(target);
    }

    return 0;
}

void Button::set_text(std::string text_){
    
    text = text_;
    textBox.setString(text);
    set_refresh();

}

}

