#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace ui {

class Text : public ContentFrame {

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

    Text(Window *master_, kwargs values = {});
    virtual ~Text();
    
    virtual int32_t set_look(std::string look_);

    int32_t draw();
    int32_t inner_reconfig();

    void set_text(std::string text_);

protected:

    Borders border;
    std::string text = "";
    sf::Text textBox;

private:
    
    int32_t setup(kwargs &values);

};

}

