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
       font (font)
    */

public:

    Text(Frame *parent_, std::map<std::string, std::string> values = {});

    int32_t draw();

    void set_text(std::string text_);

protected:

    std::string text = "";
    uint32_t style = sf::Text::Style::Regular;

private:

    int32_t setup(std::map<std::string, std::string> &values);
    float fontHeight;

};

}

