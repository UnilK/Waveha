#include "ui/knob.h"

#include <math.h>

namespace ui {

Knob::Knob(Window *master_, void *commander_, kwargs values) :
    ContentFrame(master_, values),
    commander(commander_),
    needle(sf::LineStrip, 2)
{
    set_look(look);
}

Knob::~Knob(){}

int32_t Knob::set_look(std::string look_){

    look = look_;

    radius = num("size");
    circle.setRadius(radius);
    circle.setFillColor(color("circleColor"));
    circle.setOutlineColor(color("circleBorderColor"));
    circle.setOutlineThickness(-num("circleBorderThickness"));
    
    needle[0].color = color("needleColor");
    needle[1].color = color("needleColor");

    border.set_look(look);

    inner_reconfig();
    
    return 0;
}

void Knob::set_angle(float rad){
    angle = rad;
    inner_reconfig();
    set_refresh();
}

int32_t Knob::inner_reconfig(){

    float midX = (canvasWidth - border.r + border.l) / 2;
    float midY = (canvasHeight - border.d + border.u) / 2;

    circle.setPosition(midX - radius, midY - radius);
    
    needle[0].position = {midX, midY};
    needle[1].position = {midX + std::cos(angle) * radius, midY + std::sin(angle) * radius}; 

    return 0;
}

int32_t Knob::on_event(sf::Event event, int32_t priority){

    if(priority > 0) return -1;

    if(event.type == sf::Event::MouseWheelScrolled){
        
        function(event.mouseWheelScroll.delta);

        return 1;
    }

    return -1;
}

int32_t Knob::draw(){

    border.draw(canvas);
    canvas.draw(circle);
    canvas.draw(needle);
    
    display();

    return 0;
}

}
