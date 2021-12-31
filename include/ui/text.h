#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/Text.hpp>

namespace ui{

class Text : public ContentFrame {

    /*
       style parameters:
       background (color)
       textSize (num)
       textColor (color)
       textStyle (textStyle)
       font (font)
    */

public:

    Text(Frame *parent_, kwargs values = {});

    int32_t draw();
    int32_t inner_reconfig();

    void set_text(std::string text_);

protected:

    std::string text = "";
    sf::Text textBox;

private:

    int32_t setup(kwargs &values);

};

}

