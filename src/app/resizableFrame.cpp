#include "app/resizableFrame.h"

#include <sstream>
#include <string>
#include <iostream>

ResizerFrame::ResizerFrame(ui::Frame *parent_, kwargs values) : 
    ui::ContentFrame(parent_, values)
{

    std::stringstream value;

    if(read_value("direction", value, values))
        value >> directionX >> directionY;

    background.setOutlineColor(color("borderColor"));
    background.setOutlineThickness(-num("borderThickness"));
    background.setFillColor(color("background"));
    background.setPosition(0, 0);

    dot.setRadius(num("dotSize"));
    dot.setFillColor(color("dotBackground"));
    dot.setOutlineThickness(-num("dotBorderThickness"));
    dot.setOutlineColor(color("dotBorderColor"));

    inner_reconfig();

}

int32_t ResizerFrame::inner_reconfig(){
    

    dot.setPosition(canvasWidth/2 - dot.getRadius(), canvasHeight/ 2 - dot.getRadius());
    
    background.setSize({canvasWidth, canvasHeight}); 

    return 0;
}

int32_t ResizerFrame::draw(){

    canvas.clear(color("background"));

    canvas.draw(background);
    canvas.draw(dot);

    return display();
}

int32_t ResizerFrame::on_event(sf::Event event, int32_t priority){
    
    if(priority > 0) return -1;

    if(event.type == sf::Event::MouseButtonPressed){
        
        auto [x, y] = global_mouse();
        dragX = x;
        dragY = y;
        
        auto [width, height] = parent->get_target_canvas_size();
        dragWidth = width;
        dragHeight = height;
        
        pressed = 1;
        return 1;
    }
    else if(event.type == sf::Event::MouseButtonReleased){
        pressed = 0;
        return 1;
    }
    else if(event.type == sf::Event::MouseMoved){
        if(pressed){
            
            auto [x, y] = global_mouse();

            ui::Frame *upper = get_parent(2);
            
            float minWidth = parent->get(1, 0)->targetWidth + parent->get(1, 2)->targetWidth;
            float maxWidth = upper->canvasWidth - (parent->globalX - upper->globalX);
            
            float minHeight = parent->get(0, 1)->targetHeight + parent->get(2, 1)->targetHeight;
            float maxHeight = upper->canvasHeight - (parent->globalY - upper->globalY);

            parent->set_target_size(
                    std::max(minWidth, std::min(maxWidth, dragWidth + directionX * (x - dragX))),
                    std::max(minHeight, std::min(maxHeight, dragHeight + directionY * (y - dragY))));
            
            get_parent(2)->update_grid();

            return 1;
        }
    } else if(event.type == sf::Event::MouseWheelScrolled){
        
        if(scrollable){
            
            float deltaX = 0, deltaY = 0;
            
            if(directionX != 0) deltaY = - 50 * event.mouseWheelScroll.delta;
            if(directionY != 0) deltaX = 50 * event.mouseWheelScroll.delta;

            scrolled->move_canvas(deltaX, deltaY);
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



ResizableFrame::ResizableFrame(ui::Frame *parent_, kwargs values) :
    ui::Frame(parent_, values)
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
        auto pointer = new ResizerFrame(this,
                {{"width", std::to_string(borderLeft)},
                {"look", chars("resizerLook")},
                {"direction", "-1 0"}});
        resizers.push_back(pointer);
        grid[1][0] = pointer;
    } else {
        auto pointer = new ui::SolidFrame(this,
                {{"width", std::to_string(borderLeft)},
                {"look", chars("borderLook")}});
        borders.push_back(pointer);
        grid[1][0] = pointer;
    }
    if(resizeRight){
        auto pointer = new ResizerFrame(this,
                {{"width", std::to_string(borderRight)},
                {"look", chars("resizerLook")},
                {"direction", "1 0"}});
        resizers.push_back(pointer);
        grid[1][2] = pointer;
    } else {
        auto pointer = new ui::SolidFrame(this,
                {{"width", std::to_string(borderRight)},
                {"look", chars("borderLook")}});
        borders.push_back(pointer);
        grid[1][2] = pointer;
    }
    if(resizeUp){
        auto pointer = new ResizerFrame(this,
                {{"height", std::to_string(borderUp)},
                {"look", chars("resizerLook")},
                {"direction", "0 -1"}});
        resizers.push_back(pointer);
        grid[0][1] = pointer;
    } else {
        auto pointer = new ui::SolidFrame(this,
                {{"height", std::to_string(borderUp)},
                {"look", chars("borderLook")}});
        borders.push_back(pointer);
        grid[0][1] = pointer;
    }
    if(resizeDown){
        auto pointer = new ResizerFrame(this,
                {{"height", std::to_string(borderDown)},
                {"look", chars("resizerLook")},
                {"direction", "0 1"}});
        resizers.push_back(pointer);
        grid[2][1] = pointer;
    } else {
        auto pointer = new ui::SolidFrame(this,
                {{"height", std::to_string(borderDown)},
                {"look", chars("borderLook")}});
        borders.push_back(pointer);
        grid[2][1] = pointer;
    }

    ui::SolidFrame *pointer;
    
    pointer = new ui::SolidFrame(this,
                {{"width", std::to_string(borderLeft)},
                {"height", std::to_string(borderUp)},
                {"look", chars("borderLook")}});
    borders.push_back(pointer);
    grid[0][0] = pointer;
    
    pointer = new ui::SolidFrame(this,
                {{"width", std::to_string(borderLeft)},
                {"height", std::to_string(borderDown)},
                {"look", chars("borderLook")}});
    borders.push_back(pointer);
    grid[2][0] = pointer;
    
    pointer = new ui::SolidFrame(this,
                {{"width", std::to_string(borderRight)},
                {"height", std::to_string(borderUp)},
                {"look", chars("borderLook")}});
    borders.push_back(pointer);
    grid[0][2] = pointer;
    
    pointer = new ui::SolidFrame(this,
                {{"width", std::to_string(borderRight)},
                {"height", std::to_string(borderDown)},
                {"look", chars("borderLook")}});
    borders.push_back(pointer);
    grid[2][2] = pointer;
    
}

ResizableFrame::~ResizableFrame(){
    for(auto i : resizers) delete i;
    for(auto i : borders) delete i;
}

void ResizableFrame::set_inner(ui::Frame *frame){
    grid[1][1] = frame;
}

