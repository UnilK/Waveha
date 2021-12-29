#pragma once

#include "ui/frame.h"

class ResizerFrame : public ui::ContentFrame {

    /*
       style parameters:
       dotColor (color)
       background (color)
     */

public:
    
    ResizerFrame(ui::Frame *parent_, std::map<std::string, std::string> values = {});

    int32_t draw();
    int32_t on_event(sf::Event event, int32_t priority);

protected:

    int32_t directionX = 0, directionY = 0;
    float dotSize = 5;

    float dragX = 0, dragY = 0;
    float dragWidth = 0, dragHeight = 0;

    bool pressed = 0;

};



class ResizableFrame : public ui::Frame {

    /*
       wrapper class for resizable frames.

       style parameters:
       resizerLook (look)
       borderLook (look)
     */

public:

    ResizableFrame(ui::Frame *parent_, std::map<std::string, std::string> values = {});
    ~ResizableFrame();

    void set_inner(ui::Frame *frame);

protected:

    std::vector<ResizerFrame*> resizers;
    std::vector<ui::SolidFrame*> borders;
    
    float borderLeft = 0, borderRight = 0, borderUp = 0, borderDown = 0;
    bool resizeLeft = 0, resizeRight = 0, resizeUp = 0, resizeDown = 0;

};

