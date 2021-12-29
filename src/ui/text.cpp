#include "ui/text.h"

#include <sstream>
#include <iostream>
#include <math.h>

namespace ui{

int32_t Text::setup(std::map<std::string, std::string> &values){
    
    std::stringstream value;
    
    if(read_value("text", value, values)){
        text = "";
        std::string word;
        while(std::getline(value, word)) text += word+"\n";
        if(!text.empty()) text.pop_back();
    }
    if(read_value("style", value, values)){
        
        std::string s;
        value >> s;
        
        if(s == "bold"){
            style = sf::Text::Style::Bold;
        }
    }
    
    return 0;
}

Text::Text(Frame *parent_, std::map<std::string, std::string> values) :
    ContentFrame(parent_, values)
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

int32_t Text::draw(){
   
    canvas.clear(color("background"));

    sf::Text textBox(text, font("font"), num("textSize"));
    textBox.setFillColor(color("textColor"));
    textBox.setStyle(style);
    
    sf::FloatRect rect = textBox.getLocalBounds();
  
    float textX = std::round(canvasWidth / 2 - rect.width / 2);
    float textY = std::round(canvasHeight - fontHeight - 1);

    textBox.setPosition(textX, textY);

    canvas.draw(textBox);

    display();

    return 0;
}

void Text::set_text(std::string text_){
    
    text = text_;
    refreshFlag = 1;

}

}

