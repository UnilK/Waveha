#include "ui/slider.h"

#include <iostream>
#include <assert.h>

namespace ui {

Slider::Slider(Window *master_, Side barSide, Side stackStick, bool scrollable, Kwargs kwargs) : 
    Frame(master_, kwargs),
    stack(master_, stackStick),
    buttons(master_, (barSide == Side::left || barSide == Side::right) ? Side::up : Side::left),
    functions(master_),
    bar(master_, this, &stack, barSide, scrollable),
    dot(master_),
    label(master_),
    side(barSide)
{

    set_border(0, 0, 0, 0);

    if(side == Side::left || side == Side::right){
        
        setup_grid(1, 2);

        functions.setup_grid(2, 1);
        functions.put(0, 0, &bar);
        functions.put(1, 0, &buttons);
        functions.fill_width(0, 1);
        functions.fill_height(0, 1);
        
        bar.setup_grid(2, 1);
        bar.put(0, 0, &dot);
        bar.put(1, 0, &label);
        bar.fill_width(0, 1);
        bar.fill_height(1, 1);

        label.text_stick(Text::down);
        label.text_direction(Text::down);
        label.text_offset(0, 0);
        label.set_border(1, 1, 0, 0);
    }
    else {

        setup_grid(2, 1);

        functions.setup_grid(1, 2);
        functions.put(0, 0, &bar);
        functions.put(0, 1, &buttons);
        functions.fill_width(0, 1);
        functions.fill_height(0, 1);
        
        bar.setup_grid(1, 2);
        bar.put(0, 0, &dot);
        bar.put(0, 1, &label);
        bar.fill_width(1, 1);
        bar.fill_height(0, 1);
        
        label.text_stick(Text::left);
        label.text_direction(Text::left);
        label.text_offset(0, -0.1);
        label.set_border(0, 0, 1, 1);
    }

    switch(side){
        case Side::left:

            put(0, 0, &functions);
            put(0, 1, &stack);
            fill_width(1, 1);
            fill_height(0, 1);
            
            stack.set_border(0, 1, 1, 1);

            break;
        case Side::right:

            put(0, 1, &functions);
            put(0, 0, &stack);
            fill_width(0, 1);
            fill_height(0, 1);
            
            stack.set_border(1, 0, 1, 1);

            break;
        case Side::up:

            put(0, 0, &functions);
            put(1, 0, &stack);
            fill_width(0, 1);
            fill_height(1, 1);

            stack.set_border(1, 1, 0, 1);
            
            break;
        case Side::down:
            
            put(1, 0, &functions);
            put(0, 0, &stack);
            fill_width(0, 1);
            fill_height(0, 1);

            stack.set_border(1, 1, 1, 0);
            
            break;
    }

    dot.focusable = 0;
    label.focusable = 0;

    set_look(look);
}

Slider::~Slider(){}

void Slider::set_look(std::string look_){

    look = look_;

    stack.set_look(look);
    functions.set_look(look);
    dot.set_look(look);
    label.set_look(look);

    float size = num("barSize");

    functions.set_target_size(size, size);
    bar.set_target_size(size, size);
    dot.set_target_size(size, size);

    if(side == Side::left || side == Side::right) set_range(size, 1e9, 0, 1e9);
    else set_range(0, 1e9, size, 1e9);

    set_target_size(targetWidth, targetHeight);

    on_reconfig();
}

void Slider::on_resize(sf::Mouse::Button button, float width, float height){
    
    set_target_size(width, height);

    assert(get_parent(1) != nullptr);
    get_parent(1)->update_grid();
}

void Slider::set_label(std::string s){
    label.set_text(s);
}

std::string Slider::get_label(){
    return label.get_text();
}

void Slider::overload_inner(Frame *content){
    
    switch(side){
        case Side::left:
            put(0, 1, content);
            break;
        case Side::right:
            put(0, 0, content);
            break;
        case Side::up:
            put(1, 0, content);
            break;
        case Side::down:
            put(0, 0, content);
            break;
    }
}



Slider::Dot::Dot(Window *master_, Kwargs kwargs) : Frame(master_, kwargs) {}

void Slider::Dot::set_look(std::string look_){
    
    look = look_;
    
    dot.setRadius(num("dotSize"));
    dot.setFillColor(color("dotColor"));
    dot.setOutlineColor(color("dotBorderColor"));
    dot.setOutlineThickness(-num("dotBorderThickness"));

}

void Slider::Dot::on_reconfig(){
    dot.setPosition(canvasWidth / 2 - dot.getRadius(), canvasHeight / 2 - dot.getRadius());
}

void Slider::Dot::on_refresh(){
    master->draw(dot);
}



Slider::Bar::Bar(Window *master_, Slider *slider_, Stack *stack, Side side, bool scrollable_) :
    Frame(master_),
    scrollable(scrollable_),
    slider(slider_),
    scrolled(stack)
{
    switch(side){
        case Side::left:
            directionX = -1;
            break;
        case Side::right:
            directionX = 1;
            break;
        case Side::up:
            directionY = -1;
            break;
        case Side::down:
            directionY = 1;
            break;
    }
}

Frame::Capture Slider::Bar::on_event(sf::Event event, int32_t priority){

    if(event.type == sf::Event::MouseButtonPressed){
        
        auto [x, y] = global_mouse();
        
        if(event.mouseButton.button == sf::Mouse::Left){
            
            leftDrag = {x, y};
            leftBegin = {slider->targetWidth, slider->targetHeight};
            leftPressed = 1;

            return Capture::capture;
        }
        else if(event.mouseButton.button == sf::Mouse::Right){
            
            rightDrag = {x, y};
            rightBegin = {slider->targetWidth, slider->targetHeight};
            rightPressed = 1;
            
            return Capture::capture;
        }
        
    }
    else if(event.type == sf::Event::MouseButtonReleased){
        if(event.mouseButton.button == sf::Mouse::Left){
            leftPressed = 0;
            return Capture::capture;
        }
        else if(event.mouseButton.button == sf::Mouse::Right){
            rightPressed = 0;
            return Capture::capture;
        }
    }
    else if(event.type == sf::Event::MouseMoved){
        
        auto [x, y] = global_mouse();

        if(leftPressed){
            
            slider->on_resize(sf::Mouse::Left,
                    leftBegin[0] + directionX * (x - leftDrag[0]),
                    leftBegin[1] + directionY * (y - leftDrag[1]));

            return Capture::capture;
        }
        else if(rightPressed){
            
            slider->on_resize(sf::Mouse::Right,
                    rightBegin[0] + directionX * (x - rightDrag[0]),
                    rightBegin[1] + directionY * (y - rightDrag[1]));

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

}
