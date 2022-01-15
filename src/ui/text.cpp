#include "ui/text.h"

#include <sstream>
#include <iostream>
#include <math.h>

namespace ui {

Text::Text(Window *master_, std::string text_, Kwargs kwargs) :
    Frame(master_, kwargs)
{
    text = text_;
    set_look(look);
}

Text::~Text(){}

void Text::set_look(std::string look_){
    
    look = look_;

    textBox.setString(text);
    textBox.setFont(font("font"));
    textBox.setStyle(textStyle("textStyle"));
    textBox.setCharacterSize(num("textSize"));
    textBox.setFillColor(color("textColor"));

    border.set_look(look);

    on_reconfig();
}

void Text::on_reconfig(){
    
    sf::FloatRect rect = textBox.getLocalBounds();
    
    float textX = std::round((canvasWidth - border.r + border.l) / 2 - rect.width / 2);
    float textY = std::round((canvasHeight - border.d + border.u) / 2
            - textBox.getCharacterSize() * 1.35f / 2);

    textBox.setPosition(textX, textY);

    border.set_size(canvasWidth, canvasHeight);
}

void Text::on_refresh(){
    border.draw(*master);
    master->draw(textBox);
}

void Text::set_text(std::string text_){
    
    text = text_;
    textBox.setString(text);
    on_reconfig();
    set_refresh();

}

}

