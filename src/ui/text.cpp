#include "ui/text.h"

#include <sstream>
#include <iostream>
#include <math.h>

namespace ui {

Text::Text(Window *master_, kwargs values) :
    Frame(master_, values)
{
    std::stringstream value;
    
    if(read_value("text", value, values)){
        text = "";
        std::string word;
        while(std::getline(value, word)) text += word+"\n";
        if(!text.empty()) text.pop_back();
    }

    set_look(look);
}

Text::~Text(){}

int32_t Text::set_look(std::string look_){
    
    look = look_;

    textBox.setString(text);
    textBox.setFont(font("font"));
    textBox.setStyle(textStyle("textStyle"));
    textBox.setCharacterSize(num("textSize"));
    textBox.setFillColor(color("textColor"));

    border.set_look(look);

    on_reconfig();

    return 0;
}

int32_t Text::on_reconfig(){
    
    sf::FloatRect rect = textBox.getLocalBounds();
    
    float textX = std::round((canvasWidth - border.r + border.l) / 2 - rect.width / 2);
    float textY = std::round((canvasHeight - border.d + border.u) / 2
            - textBox.getCharacterSize() * 1.35f / 2);

    textBox.setPosition(textX, textY);

    border.set_size(canvasWidth, canvasHeight);
    
    return 0;
}

int32_t Text::draw(){
   
    border.draw(*master);
    master->draw(textBox);

    return 0;
}

void Text::set_text(std::string text_){
    
    text = text_;
    textBox.setString(text);
    set_refresh();

}

}

