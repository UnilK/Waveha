#include "ui/scroll.h"

namespace ui {

Scroll::Scroll(Window *master_,std::function<void(float)> func, Kwargs kwargs) :
    Frame(master_, kwargs),
    function(func)
{}

Scroll::~Scroll(){}

Frame::Capture Scroll::on_event(sf::Event event, int priority){

    if(priority > 0) return Capture::pass;

    if(event.type == sf::Event::MouseWheelScrolled){
        
        function(event.mouseWheelScroll.delta);

        return Capture::capture;
    }

    return Capture::pass;
}

}
