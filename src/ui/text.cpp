#include "ui/text.h"

#include <math.h>
#include <algorithm>

namespace ui {

Text::Text(Window *master_, std::string text_, Kwargs kwargs) :
    Frame(master_, kwargs)
{
    text = text_;
    textBox.setString(text);

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
   
    Frame::on_reconfig();
    
    float x = offsetX * textBox.getCharacterSize();
    float y = offsetY * textBox.getCharacterSize();

    float textX = 0;
    float textY = 0;

    switch(stick){
        case left:
            textX = std::round(border.l + x);
            textY = std::round(y - border.d);
            break;
        case down:
            textX = std::round((canvasWidth - border.r + border.l + x - rect.width - rect.left) / 2);
            textY = std::round(border.u + y);
            break;
        case middle:
            textX = std::round((canvasWidth - border.r + border.l + x - rect.width - rect.left) / 2);
            textY = std::round((canvasHeight - border.d + border.u + y - rect.height - rect.top) / 2);
            break;
    }

    textBox.setPosition(textX, textY);
}

void Text::on_refresh(){
    Frame::on_refresh();
    master->draw(textBox);
}

void Text::set_text(std::string text_){
   
    text = text_;

    switch(direction){
        case left:
        case middle:
            textBox.setString(text);
            break;
        case down:
            std::string vtex;
            for(char i : text){
                vtex.push_back(i);
                vtex.push_back('\n');
            }
            vtex.pop_back();
            textBox.setString(vtex);
            break;
    }

    on_reconfig();
    set_refresh();
}

std::string Text::get_text(){
    return text;
}

void Text::text_stick(Position s){
    stick = s;
}

void Text::text_direction(Position d){
    direction = d;
}

void Text::text_offset(float x, float y){
    offsetX = x;
    offsetY = y;
}

}
