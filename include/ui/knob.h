#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace ui {

class Knob : public Frame {
    
public:

    Knob(Window *master_, void *commander_, Kwargs = {});
    virtual ~Knob();

    void set_look(std::string look_);
    void on_reconfig();
    Capture on_event(sf::Event event, int32_t priority);
    void on_refresh();

    void set_angle(float);

protected:

    void *commander = nullptr;

    virtual void function(float delta) = 0;
    
    Borders border;

    float radius, angle = 0;
    sf::CircleShape circle;
    sf::VertexArray needle;

};

}
