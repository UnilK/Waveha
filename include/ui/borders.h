#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace ui{

class Frame;

class Borders {

public:

    void set_look(Frame *parent);
    void set_size(float width, float height);
    void set_position(float x, float y);
    void draw(sf::RenderTarget &target);

    sf::RectangleShape back, left, right, up, down;
    float w = 0, h = 0;
    float l = 0, r = 0, u = 0, d = 0;

};

}