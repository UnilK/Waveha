#include "ui/text.h"

#include <sstream>
#include <iostream>
#include <math.h>

namespace ui{

int32_t Text::setup(kwargs &values){
    
    std::stringstream value;
    
    if(read_value("text", value, values)){
        text = "";
        std::string word;
        while(std::getline(value, word)) text += word+"\n";
        if(!text.empty()) text.pop_back();
    }
    
    return 0;
}

Text::Text(Frame *parent_, kwargs values) :
    ContentFrame(parent_, values)
{
    setup(values);

    textBox.setString(text);
    textBox.setFont(font("font"));
    textBox.setStyle(textStyle("textStyle"));
    textBox.setCharacterSize(num("textSize"));
    textBox.setFillColor(color("textColor"));

    inner_reconfig();

    canvas.setSmooth(0);
}

int32_t Text::inner_reconfig(){

    sf::FloatRect rect = textBox.getLocalBounds();

    float textX = std::round(canvasWidth / 2 - rect.width / 2);
    float textY = std::round(canvasHeight / 2 - textBox.getCharacterSize() * 1.35f / 2);

    textBox.setPosition(textX, textY);

    return 0;
}

int32_t Text::draw(){
   
    canvas.clear(color("background"));

    canvas.draw(textBox);

    display();

    return 0;
}

void Text::set_text(std::string text_){
    
    text = text_;
    textBox.setString(text);
    refreshFlag = 1;

}

}

