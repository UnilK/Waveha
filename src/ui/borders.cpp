#include "ui/borders.h"
#include "ui/frame.h"

#include <iostream>
#include <algorithm>

namespace ui {

Borders::Borders() :
    w(0), h(0), l(0), r(0), u(0), d(0), isl(0), isr(0), isu(0), isd(0)
{}

Borders::~Borders(){}

void Borders::set_look(std::string look_){

    look = look_;

    back.setFillColor(color("background"));
    left.setFillColor(color("borderColor"));
    right.setFillColor(color("borderColor"));
    up.setFillColor(color("borderColor"));
    down.setFillColor(color("borderColor"));

    auto borders = nums("borderThickness");
    if(borders.size() == 1){
        l = isl ? borders[0] : 0;
        r = isr ? borders[0] : 0;
        u = isu ? borders[0] : 0;
        d = isd ? borders[0] : 0;
    } else if(borders.size() == 4){
        l = isl ? borders[0] : 0;
        r = isr ? borders[1] : 0;
        u = isu ? borders[2] : 0;
        d = isd ? borders[3] : 0;
    } else {
        l = r = u = d = 0;
    }
}

void Borders::set_border(bool il, bool ir, bool iu, bool id){
    isl = il;
    isr = ir;
    isu = iu;
    isd = id;
}

void Borders::set_size(float width, float height){

    w = width;
    h = height;

    back.setSize({w, h});
    left.setSize({std::min(l, w), h});
    right.setSize({std::min(r, w), h});
    up.setSize({w, std::min(u, h)});
    down.setSize({w, std::min(d, h)});
    
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

    bool okBack = back.getFillColor().a != 0;
    bool okBorder= left.getFillColor().a != 0;
    
    if(w > 0 && h > 0 && okBack) target.draw(back);
    if(l > 0 && h > 0 && okBorder) target.draw(left);
    if(r > 0 && h > 0 && okBorder) target.draw(right);
    if(w > 0 && u > 0 && okBorder) target.draw(up);
    if(w > 0 && d > 0 && okBorder) target.draw(down);

}

void Borders::update(){
    set_look(look);
}

}

