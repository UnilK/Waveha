#include "app/resizableFrame.h"

#include <sstream>
#include <string>
#include <iostream>

namespace app {

ResizerFrame::ResizerFrame(ui::Window *master_, kwargs values) : 
    ui::ContentFrame(master_, values)
{

    std::stringstream value;

    if(read_value("direction", value, values))
        value >> directionX >> directionY;

    set_look(look);
}

int32_t ResizerFrame::set_look(std::string look_){
    
    look = look_;

    dot.setRadius(num("dotSize"));
    dot.setFillColor(color("dotBackground"));
    dot.setOutlineThickness(-num("dotBorderThickness"));
    dot.setOutlineColor(color("dotBorderColor"));

    border.set_look(look);

    inner_reconfig();

    return 0;
}

int32_t ResizerFrame::inner_reconfig(){
    
    dot.setPosition(canvasWidth/2 - dot.getRadius(), canvasHeight/ 2 - dot.getRadius());
    
    border.set_size(canvasWidth, canvasHeight); 

    return 0;
}

int32_t ResizerFrame::draw(){

    canvas.clear(color("background"));

    border.draw(canvas);
    canvas.draw(dot);

    return display();
}

int32_t ResizerFrame::on_event(sf::Event event, int32_t priority){
    
    if(priority > 0) return -1;

    if(event.type == sf::Event::MouseButtonPressed){
        
        if(event.mouseButton.button == sf::Mouse::Left){
            
            auto [x, y] = global_mouse();
            dragX = x;
            dragY = y;
            
            auto [width, height] = parent->get_target_canvas_size();
            dragWidth = width;
            dragHeight = height;
            
            resizerPressed = 1;

            return 1;
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased){

        if(event.mouseButton.button == sf::Mouse::Left){
            resizerPressed = 0;
            return 1;
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

            return 1;
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

            return 1;
        }
    }

    return -1;
}

void ResizerFrame::set_scrollable(bool scrollable_, ui::Frame *scrolled_){
    scrollable = scrollable_;
    scrolled = scrolled_;
}



ResizableFrame::ResizableFrame(ui::Window *master_, kwargs values) :
    ui::Frame(master_, values)
{
    
    std::stringstream value;
    
    if(read_value("border", value, values))
        value >> borderLeft >> borderRight >> borderUp >> borderDown;
    if(read_value("resize", value, values))
        value >> resizeLeft >> resizeRight >> resizeUp >> resizeDown;
    
    setup_grid(3, 3);

    fill_width({0, 1, 0});
    fill_height({0, 1, 0});

    if(resizeLeft){
        auto pointer = new ResizerFrame(master, kwargs
                {{"width", std::to_string(borderLeft)},
                {"look", chars("resizerLook")},
                {"direction", "-1 0"}});
        resizers.push_back(pointer);
        put(1, 0, pointer);
    } else {
        auto pointer = new ui::SolidFrame(master, kwargs
                {{"width", std::to_string(borderLeft)},
                {"look", chars("borderLook")}});
        borders.push_back(pointer);
        put(1, 0, pointer);
    }
    if(resizeRight){
        auto pointer = new ResizerFrame(master, kwargs
                {{"width", std::to_string(borderRight)},
                {"look", chars("resizerLook")},
                {"direction", "1 0"}});
        resizers.push_back(pointer);
        put(1, 2, pointer);
    } else {
        auto pointer = new ui::SolidFrame(master, kwargs
                {{"width", std::to_string(borderRight)},
                {"look", chars("borderLook")}});
        borders.push_back(pointer);
        put(1, 2, pointer);
    }
    if(resizeUp){
        auto pointer = new ResizerFrame(master, kwargs
                {{"height", std::to_string(borderUp)},
                {"look", chars("resizerLook")},
                {"direction", "0 -1"}});
        resizers.push_back(pointer);
        put(0, 1, pointer);
    } else {
        auto pointer = new ui::SolidFrame(master, kwargs
                {{"height", std::to_string(borderUp)},
                {"look", chars("borderLook")}});
        borders.push_back(pointer);
        put(0, 1, pointer);
    }
    if(resizeDown){
        auto pointer = new ResizerFrame(master, kwargs
                {{"height", std::to_string(borderDown)},
                {"look", chars("resizerLook")},
                {"direction", "0 1"}});
        resizers.push_back(pointer);
        put(2, 1, pointer);
    } else {
        auto pointer = new ui::SolidFrame(master, kwargs
                {{"height", std::to_string(borderDown)},
                {"look", chars("borderLook")}});
        borders.push_back(pointer);
        put(2, 1, pointer);
    }

    ui::SolidFrame *pointer;
    
    pointer = new ui::SolidFrame(master, kwargs
                {{"width", std::to_string(borderLeft)},
                {"height", std::to_string(borderUp)},
                {"look", chars("borderLook")}});
    borders.push_back(pointer);
    put(0, 0, pointer);
    
    pointer = new ui::SolidFrame(master, kwargs
                {{"width", std::to_string(borderLeft)},
                {"height", std::to_string(borderDown)},
                {"look", chars("borderLook")}});
    borders.push_back(pointer);
    put(2, 0, pointer);
    
    pointer = new ui::SolidFrame(master, kwargs
                {{"width", std::to_string(borderRight)},
                {"height", std::to_string(borderUp)},
                {"look", chars("borderLook")}});
    borders.push_back(pointer);
    put(0, 2, pointer);
    
    pointer = new ui::SolidFrame(master, kwargs
                {{"width", std::to_string(borderRight)},
                {"height", std::to_string(borderDown)},
                {"look", chars("borderLook")}});
    borders.push_back(pointer);
    put(2, 2, pointer);
    
}

ResizableFrame::~ResizableFrame(){
    for(auto i : resizers) delete i;
    for(auto i : borders) delete i;
}

void ResizableFrame::set_inner(ui::Frame *frame){
    put(1, 1, frame);
}

}
