#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/Text.hpp>

namespace ui {


class Text : public Frame {

    /*
       style:

       background (color)
       borderColor (color)
       borderThickness (num) or (num(left) num(right) num(up) num(down))
       
       textSize (num)
       textColor (color)
       textStyle (textStyle)
       font (font)
    */

public:

    Text(Window *master_, std::string text_ = "", Kwargs = {});
    virtual ~Text();
    
    virtual void set_look(std::string look_);
    void on_reconfig();
    void on_refresh();

    void set_text(std::string text_);
    std::string get_text();
    
    enum Position { left, down, middle};

    // use these for initialization.
    void text_stick(Position);
    void text_direction(Position);
    void text_offset(float x, float y);
    // offset is relative to character size: x -> x * characterSize.


private:

    std::string text;
    
    Position stick = Position::middle, direction = Position::left;

    float offsetX = 0, offsetY = -0.3;

    sf::Text textBox;

};

}

