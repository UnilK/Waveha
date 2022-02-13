#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <functional>

namespace ui {

class Scroll : public Frame {
    
public:

    Scroll(Window *master_, std::function<void(float)> func, Kwargs = {});
    virtual ~Scroll();

    Capture on_event(sf::Event event, int priority);

private:

    std::function<void(float)> function;

};

}
