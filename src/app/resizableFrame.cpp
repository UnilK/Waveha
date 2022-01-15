#include "app/resizableFrame.h"

#include <string>
#include <iostream>

namespace app {

ResizerFrame::ResizerFrame(ui::Window *master_, float dX, float dY, Kwargs kwargs) : 
    ui::Frame(master_, kwargs)
{
    directionX = dX;
    directionY = dY;
    set_look(look);
}

void ResizerFrame::set_look(std::string look_){
    
    look = look_;

    dot.setRadius(num("dotSize"));
    dot.setFillColor(color("dotBackground"));
    dot.setOutlineThickness(-num("dotBorderThickness"));
    dot.setOutlineColor(color("dotBorderColor"));

    border.set_look(look);

    on_reconfig();
}

void ResizerFrame::on_reconfig(){
    dot.setPosition(canvasWidth/2 - dot.getRadius(), canvasHeight/ 2 - dot.getRadius());
    border.set_size(canvasWidth, canvasHeight); 
}

void ResizerFrame::on_refresh(){
    border.draw(*master);
    master->draw(dot);
}

ui::Frame::Capture ResizerFrame::on_event(sf::Event event, int32_t priority){
    
    if(priority > 0) return Capture::pass;

    if(event.type == sf::Event::MouseButtonPressed){
        
        if(event.mouseButton.button == sf::Mouse::Left){
            
            auto [x, y] = global_mouse();
            dragX = x;
            dragY = y;
            
            dragWidth = parent->targetWidth;
            dragHeight = parent->targetHeight;
            
            resizerPressed = 1;

            return Capture::capture;
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased){

        if(event.mouseButton.button == sf::Mouse::Left){
            resizerPressed = 0;
            return Capture::capture;
        }
    }
    else if(event.type == sf::Event::MouseMoved){
        if(resizerPressed){
            
            auto [x, y] = global_mouse();

            ui::Frame *upper = get_parent(2);
            
            float minWidth =
                std::max(parent->windowX, parent->get(1, 0)->targetWidth)
                + parent->get(1, 2)->targetWidth;

            float maxWidth =
                upper->canvasWidth
                + parent->windowX
                - (parent->globalX - upper->globalX);
            
            float minHeight =
                std::max(parent->windowY, parent->get(0, 1)->targetHeight)
                + parent->get(2, 1)->targetHeight;
            
            float maxHeight =
                upper->canvasHeight
                + parent->windowY
                - (parent->globalY - upper->globalY);

            parent->set_target_size(
                    std::max(minWidth, std::min(maxWidth, dragWidth + directionX * (x - dragX))),
                    std::max(minHeight, std::min(maxHeight, dragHeight + directionY * (y - dragY))));
            
            get_parent(2)->update_grid();

            return Capture::capture;
        }
    } else if(event.type == sf::Event::MouseWheelScrolled){
        
        if(scrollable){

            float scrollSpeed = 50;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                scrollSpeed = 1;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                scrollSpeed = 100;

            
            float deltaX = 0, deltaY = 0;
            
            if(directionX != 0) deltaY = - scrollSpeed * event.mouseWheelScroll.delta;
            if(directionY != 0) deltaX = scrollSpeed * event.mouseWheelScroll.delta;

            scrolled->set_canvas_position(
                    std::max(0.0f, scrolled->canvasX + deltaX),
                    std::max(0.0f, scrolled->canvasY + deltaY));
            scrolled->update_grid();

            return Capture::capture;
        }
    }

    return Capture::pass;
}

void ResizerFrame::set_scrolled(ui::Frame *scrolled_){
    if(scrolled_ == nullptr) scrollable = 0;
    else scrollable = 1;
    scrolled = scrolled_;
}



ResizableFrame::ResizableFrame(
        ui::Window *master_,
        std::array<float, 4> border,
        std::array<bool, 4> resize,
        Kwargs kwargs) :
    ui::Frame(master_, kwargs)
{

    borderLeft = border[0];
    borderRight = border[1];
    borderUp = border[2];
    borderDown = border[3];
    
    resizeLeft = resize[0];
    resizeRight = resize[1];
    resizeUp = resize[2];
    resizeDown = resize[3];
    
    setup_grid(3, 3);

    fill_width({0, 1, 0});
    fill_height({0, 1, 0});

    if(resizeLeft){
        resizers.push_back(new ResizerFrame(master, -1, 0,
                {.look = chars("resizerLook"), .width = borderLeft}));
        put(1, 0, resizers.back());
    } else {
        borders.push_back(new ui::SolidFrame(master,
                    {.look = chars("borderLook"), .width = borderLeft}));
        put(1, 0, borders.back());
    }
    if(resizeRight){
        resizers.push_back(new ResizerFrame(master, 1, 0,
                {.look = chars("resizerLook"), .width = borderRight}));
        put(1, 2, resizers.back());
    } else {
        borders.push_back(new ui::SolidFrame(master,
                    {.look = chars("borderLook"), .width = borderRight}));
        put(1, 2, borders.back());
    }
    if(resizeUp){
        resizers.push_back(new ResizerFrame(master, 0, -1,
                {.look = chars("resizerLook"), .height = borderUp}));
        put(0, 1, resizers.back());
    } else {
        borders.push_back(new ui::SolidFrame(master,
                    {.look = chars("borderLook"), .height = borderUp}));
        put(0, 1, borders.back());
    }
    if(resizeDown){
        resizers.push_back(new ResizerFrame(master, 0, 1,
                {.look = chars("resizerLook"), .height = borderDown}));
        put(2, 1, resizers.back());
    } else {
        borders.push_back(new ui::SolidFrame(master,
                    {.look = chars("borderLook"), .height = borderDown}));
        put(2, 1, borders.back());
    }
    
    borders.push_back(new ui::SolidFrame(master,
                {.look = chars("borderLook"), .width = borderLeft, .height = borderUp}));
    put(0, 0, borders.back());
    borders.push_back(new ui::SolidFrame(master,
                {.look = chars("borderLook"), .width = borderRight, .height = borderUp}));
    put(0, 2, borders.back());
    borders.push_back(new ui::SolidFrame(master,
                {.look = chars("borderLook"), .width = borderLeft, .height = borderDown}));
    put(2, 0, borders.back());
    borders.push_back(new ui::SolidFrame(master,
                {.look = chars("borderLook"), .width = borderRight, .height = borderDown}));
    put(2, 2, borders.back());

}

ResizableFrame::~ResizableFrame(){
    for(auto i : resizers) delete i;
    for(auto i : borders) delete i;
}

void ResizableFrame::set_inner(ui::Frame *frame){
    put(1, 1, frame);
}

}
