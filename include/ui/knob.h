#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace ui {

class Knob : public Frame {
    
public:

    Knob(Window *master_, void *commander_, kwargs values = {});
    virtual ~Knob();

    int32_t set_look(std::string look_);
    
    int32_t on_reconfig();

    int32_t on_event(sf::Event event, int32_t priority);

    void set_angle(float);

    int32_t draw();

protected:

    void *commander = nullptr;

    virtual void function(float delta) = 0;
    
    Borders border;

    float radius, angle = 0;
    sf::CircleShape circle;
    sf::VertexArray needle;

};

}
