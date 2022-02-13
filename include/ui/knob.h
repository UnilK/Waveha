#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <functional>

namespace ui {

class Knob : public Frame {

    /*
       style:
       
       dotSize (num)
       dotColor (color)
       dotBorderColor (color)
       dotBorderThickness (num)

       needleColor (color)
       
       borderColor (color)
       borderThickness (num) or (num(left) num(right) num(up) num(down))
       background (color)
     */

public:

    Knob(Window *master_, std::function<void(float)> func, Kwargs = {});
    virtual ~Knob();

    void set_look(std::string look_);
    void on_reconfig();
    Capture on_event(sf::Event event, int priority);
    void on_refresh();

    void set_angle(float);

private:

    std::function<void(float)> function;
    
    Borders border;

    float radius, angle = 0;
    sf::CircleShape circle;
    sf::VertexArray needle;

};

}
