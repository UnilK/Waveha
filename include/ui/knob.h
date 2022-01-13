#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace ui {

class Knob : public ContentFrame {
    
public:

    Knob(Window *master_, void *commander_, kwargs = {});
    virtual ~Knob();

    int32_t set_look(std::string look_);
    
    int32_t on_event(sf::Event event, int32_t priority);

protected:

    void *commander = nullptr;

    virtual void function(float delta) = 0;
    
    Borders border;
    std::string text = "";

private:

    sf::CircleShape circle;
    sf::VertexArray needle;
    sf::Text textBox;

};

}
