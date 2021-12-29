#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/Text.hpp>

namespace ui{

class Button : public ui::ContentFrame {
    
    /*
       style parameters:
       background (color)
       pressedBackground (color)
       border (color)
       textSize (num)
       textColor (color)
       font (font)
    */

public:

    Button(Frame *parent_, int32_t (*function_)(void*), void *target_,
            std::map<std::string, std::string> values = {});

    int32_t draw();
    int32_t on_event(sf::Event event, int32_t priority);

    void set_text(std::string text_);

protected:

    std::string text = "";
    float borderLeft = 0, borderRight = 0, borderUp = 0, borderDown = 0;
    
    
    uint32_t style = sf::Text::Style::Regular;
    float fontHeight;

    int32_t (*function)(void*);
    void *target;
    bool pressed = 0;

private:

    int32_t setup(std::map<std::string, std::string> &values);

};

}

