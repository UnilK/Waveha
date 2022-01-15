#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

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

       kwargs:
       text (chars)
    */

public:

    Text(Window *master_, std::string text_ = "", Kwargs = {});
    virtual ~Text();
    
    virtual void set_look(std::string look_);
    void on_reconfig();
    void on_refresh();

    void set_text(std::string text_);

protected:

    Borders border;
    std::string text = "";
    sf::Text textBox;

};

}

