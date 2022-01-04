#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class ResizerFrame : public ui::ContentFrame {

    /*
       style:
       background (color)
       borderColor (color)
       borderThickness (num) or (num(left) num(right) num(up) num(down))
       dotBackground (color)
       dotSize (num)
       dotBorderThickness (num)
       dotBorderColor (color)

       kwargs:
       direction (num num)
     */

public:
    
    ResizerFrame(ui::Frame *parent_, kwargs values = {});

    int32_t set_look(std::string look_);

    int32_t draw();
    int32_t on_event(sf::Event event, int32_t priority);

    void set_scrollable(bool scrollable_, ui::Frame *scrolled_ = nullptr);
    
    int32_t inner_reconfig();

protected:

    sf::RectangleShape background;
    sf::CircleShape dot;

    int32_t directionX = 0, directionY = 0;

    float dragX = 0, dragY = 0;
    float dragWidth = 0, dragHeight = 0;

    bool scrollable = 0;
    ui::Frame *scrolled = nullptr;

    ui::Borders border;

    bool pressed = 0;

};



class ResizableFrame : public ui::Frame {

    /*
       wrapper class for resizable frames.

       style parameters:
       resizerLook (look)
       borderLook (look)

       kwargs:
       border (num(left) num(right) num(up) num(down))
       resize (bool(left) bool(right) bool(up) bool(down))
     */

public:

    ResizableFrame(ui::Frame *parent_, kwargs values = {});
    ~ResizableFrame();

    void set_inner(ui::Frame *frame);

protected:

    std::vector<ResizerFrame*> resizers;
    std::vector<ui::SolidFrame*> borders;
    
    float borderLeft = 0, borderRight = 0, borderUp = 0, borderDown = 0;
    bool resizeLeft = 0, resizeRight = 0, resizeUp = 0, resizeDown = 0;

};

