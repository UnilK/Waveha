#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/Text.hpp>

namespace ui{

class Button : public ui::ContentFrame {
    
    /*
       style parameters:
       background (color)
       pressedBackground (color)
       borderColor (color)
       borderThicness (num) or (num, num, num, num)
       textSize (num)
       textColor (color)
       textStyle (textStyle)
       font (font)
    */

public:

    Button(Frame *parent_, int32_t (*function_)(void*), void *target_, kwargs values = {});

    int32_t draw();
    int32_t inner_reconfig();
    int32_t on_event(sf::Event event, int32_t priority);

    void set_text(std::string text_);

protected:

    std::string text = "";
    float borderLeft = 0, borderRight = 0, borderUp = 0, borderDown = 0;
    
    sf::Text textBox;

    int32_t (*function)(void*);
    void *target;
    bool pressed = 0;

private:
    
    sf::RectangleShape left, right, up, down;

    int32_t setup(kwargs &values);

};

}

