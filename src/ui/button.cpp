#include "ui/button.h"

#include <iostream>
#include <sstream>
#include <math.h>

namespace ui{
    
Button::Button(Frame *parent_, int32_t(*function_)(void *),
            void *target_, std::map<std::string, std::string> values) :
    ContentFrame(parent_, values),
    function(function_),
    target(target_)
{
    setup(values);
    
    sf::Text textBox(
            "012345678j9abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOP"
            "QRSTUVWXYZ!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~",
            font("font"), num("textSize"));
    textBox.setFillColor(color("textColor"));
    textBox.setStyle(style);
    sf::FloatRect rect = textBox.getGlobalBounds();
    fontHeight = rect.height + rect.top;

    canvas.setSmooth(0);
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
    
    if(read_value("style", value, values)){
        
        std::string s;
        value >> s;
        
        if(s == "bold"){
            style = sf::Text::Style::Bold;
        }
    }
    return 0;
}

int32_t Button::draw(){

    if(pressed) canvas.clear(color("pressedBackground"));
    else canvas.clear(color("background"));

    sf::RectangleShape left({borderLeft, canvasHeight});
    sf::RectangleShape right({borderRight, canvasHeight});
    sf::RectangleShape up({canvasWidth, borderUp});
    sf::RectangleShape down({canvasWidth, borderDown});

    sf::Text textBox(text, font("font"), num("textSize"));
    textBox.setFillColor(color("textColor"));
    
    sf::FloatRect rect = textBox.getGlobalBounds();
    
    float textX = std::round((canvasWidth - borderRight + borderLeft) / 2 - rect.width / 2);
    float textY = std::round((canvasHeight - borderDown + borderUp - fontHeight - 1));

    textBox.setPosition(textX, textY);

    left.setPosition(0, 0);
    right.setPosition(canvasWidth - borderRight, 0);
    up.setPosition(0, 0);
    down.setPosition(0, canvasHeight - borderDown);

    left.setFillColor(color("border"));
    right.setFillColor(color("border"));
    up.setFillColor(color("border"));
    down.setFillColor(color("border"));

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
        refreshFlag = 1;
    }
    else if(event.type == sf::Event::MouseButtonReleased){
        pressed = 0;
        refreshFlag = 1;
        if(master->get_soft_focus() == this)
            function(target);
    }

    return 0;
}

void Button::set_text(std::string text_){
    
    text = text_;
    refreshFlag = 1;

}

}

