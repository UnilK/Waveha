#include "ui/knob.h"

#include <math.h>

namespace ui {

Knob::Knob(Window *master_,std::function<void(float)> func, Kwargs kwargs) :
    Frame(master_, kwargs),
    function(func),
    needle(sf::LineStrip, 2)
{
    set_look(look);
}

Knob::~Knob(){}

void Knob::set_look(std::string look_){

    look = look_;

    radius = num("dotSize");
    circle.setRadius(radius);
    circle.setFillColor(color("dotColor"));
    circle.setOutlineColor(color("dotBorderColor"));
    circle.setOutlineThickness(-num("dotBorderThickness"));
    
    needle[0].color = color("needleColor");
    needle[1].color = color("needleColor");

    border.set_look(look);

    set_reconfig();
}

void Knob::on_reconfig(){

    float midX = (canvasWidth - border.r + border.l) / 2;
    float midY = (canvasHeight - border.d + border.u) / 2;

    circle.setPosition(midX - radius, midY - radius);
    
    needle[0].position = {midX, midY};
    needle[1].position = {midX + std::cos(angle) * radius, midY + std::sin(angle) * radius}; 
}

Frame::Capture Knob::on_event(sf::Event event, int32_t priority){

    if(priority > 0) return Capture::pass;

    if(event.type == sf::Event::MouseWheelScrolled){
        
        function(event.mouseWheelScroll.delta);

        return Capture::capture;
    }

    return Capture::pass;
}

void Knob::on_refresh(){
    border.draw(*master);
    master->draw(circle);
    master->draw(needle);
}

void Knob::set_angle(float rad){
    angle = rad;
    on_reconfig();
    set_refresh();
}


}
