#pragma once

#include "ui/style.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace ui {

class Frame;

class Borders : public Styled {

public:

    int32_t set_look(std::string look_);
    void set_size(float width, float height);
    void set_position(float x, float y);
    void draw(sf::RenderTarget &target);

    sf::RectangleShape back, left, right, up, down;
    float w = 0, h = 0;
    float l = 0, r = 0, u = 0, d = 0;

};

}
