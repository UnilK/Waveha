#include "ui/borders.h"
#include "ui/frame.h"

#include <sstream>

namespace ui {

void Borders::set_look(Frame *parent){

    back.setFillColor(parent->color("background"));
    left.setFillColor(parent->color("borderColor"));
    right.setFillColor(parent->color("borderColor"));
    up.setFillColor(parent->color("borderColor"));
    down.setFillColor(parent->color("borderColor"));

    std::stringstream probe(parent->chars("borderThickness"));
    std::stringstream borders(parent->chars("borderThickness"));
    std::string thickness;
    int32_t total = 0;

    while(probe >> thickness) total++;

    if(total == 1){
        borders >> d;
        l = r = u = d;
    } else if(total == 4){
        borders >> l >> r >> u >> d;
    }

}

void Borders::set_size(float width, float height){

    w = width;
    h = height;

    back.setSize({w, h});
    left.setSize({l, h});
    right.setSize({r, h});
    up.setSize({w, u});
    down.setSize({w, d});
    
    back.setPosition(0, 0);
    left.setPosition(0, 0);
    right.setPosition(w - r, 0);
    up.setPosition(0, 0);
    down.setPosition(0, h - d);
    
}

void Borders::set_position(float x, float y){
   
    back.setPosition(x, y);
    left.setPosition(x, y);
    right.setPosition(x + w - r, y);
    up.setPosition(x, y);
    down.setPosition(x, y + h - d);

}

void Borders::draw(sf::RenderTarget &target){

    target.draw(back);
    target.draw(left);
    target.draw(right);
    target.draw(up);
    target.draw(down);

}

}

