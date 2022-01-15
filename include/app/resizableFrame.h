#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace app {

using ui::Kwargs;

class ResizerFrame : public ui::Frame {

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
    
    ResizerFrame(ui::Window *master_, float dX, float dY, Kwargs = {});

    void set_look(std::string look_);
    Capture on_event(sf::Event event, int32_t priority);
    void on_reconfig();
    void on_refresh();

    void set_scrolled(ui::Frame *scrolled_);

private:

    sf::RectangleShape background;
    sf::CircleShape dot;

    float directionX = 0, directionY = 0;

    float dragX = 0, dragY = 0;
    float dragWidth = 0, dragHeight = 0;

    bool resizerPressed = 0;
    bool scrollable = 0;
    ui::Frame *scrolled = nullptr;

    ui::Borders border;

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

    ResizableFrame(
            ui::Window *parent_,
            std::array<float, 4> border,
            std::array<bool, 4> resize,
            Kwargs = {});
    ~ResizableFrame();

    void set_inner(ui::Frame *frame);

private:

    std::vector<ResizerFrame*> resizers;
    std::vector<ui::SolidFrame*> borders;
    
    float borderLeft = 0, borderRight = 0, borderUp = 0, borderDown = 0;
    bool resizeLeft = 0, resizeRight = 0, resizeUp = 0, resizeDown = 0;

};

}
