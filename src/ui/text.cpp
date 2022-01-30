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

    set_reconfig();
}

void Text::on_reconfig(){
    
    sf::FloatRect rect = textBox.getLocalBounds();
   
    Frame::on_reconfig();
    
    float x = offsetX * textBox.getCharacterSize();
    float y = offsetY * textBox.getCharacterSize();

    float textX = 0;
    float textY = 0;

    float lX = std::round(border.l + x);
    float mX = std::round((canvasWidth - border.r + border.l - rect.width - rect.left) / 2 + x);
    float rX = std::round(canvasHeight + canvasWidth - rect.width - border.r + x);
    float uY = std::round(border.u + y);
    float mY = std::round((canvasHeight - border.d + border.u - textBox.getCharacterSize()) / 2
            - textBox.getFont()->getUnderlinePosition(textBox.getCharacterSize()) + y);
    float dY = std::round(canvasHeight - textBox.getCharacterSize()
            - textBox.getFont()->getUnderlinePosition(textBox.getCharacterSize()) + y - border.d);

    switch(stick){
        case left:
            textX = lX;
            textY = mY;
            break;
        case right:
            textX = rX;
            textY = mY;
            break;
        case up:
            textX = mX;
            textY = uY;
            break;
        case down:
            textX = mX;
            textY = dY;
            break;
        case leftup:
            textX = lX;
            textY = uY;
            break;
        case rightup:
            textX = rX;
            textY = uY;
            break;
        case leftdown:
            textX = lX;
            textY = dY;
            break;
        case rightdown:
            textX = rX;
            textY = dY;
            break;
        case middle:
            textX = mX;
            textY = mY;
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
        case right:
        case leftup: 
        case rightup:
        case leftdown:
        case rightdown:
            textBox.setString(text);
            break;
        case down:
        case up:
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
