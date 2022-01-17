#include "ui/window.h"
#include "ui/clock.h"

#include <iostream>
#include <sstream>
#include <math.h>

namespace ui {

Window::Window(float width, float height, std::string title) :
    sf::RenderWindow(sf::VideoMode(width, height), title),
    Frame(this)
{
    previousFrame.create(width, height);
    setVerticalSyncEnabled(1);
}

Window::~Window(){}

void Window::event_update(){
    
    sf::Event event;
    while (pollEvent(event)){

        if(event.type == sf::Event::Closed){
                
            close();
            on_close();
            
            destroyed = 1;

        } else if(event.type == sf::Event::Resized){
                
            float width = event.size.width;
            float height = event.size.height;

            setView(sf::View(sf::FloatRect(0, 0, width, height)));
            previousFrame.create(width, height);

            set_window_size(width, height);
            set_canvas_size(width, height);
            update_grid();

            resizeFlag = 1;
            
            on_resize();
                
        } else if(event.type == sf::Event::MouseMoved){
                
            mouseX = event.mouseMove.x;
            mouseY = event.mouseMove.y;

            focus = find_focus();
            softFocus = focus.back();
                
        } else {

            if(event.type == sf::Event::MouseButtonPressed){
                
                mouseX = event.mouseButton.x;
                mouseY = event.mouseButton.y;
                
                set_hard();
            }
            
            if(event.type == sf::Event::MouseWheelScrolled){
                
                mouseX = event.mouseWheelScroll.x;
                mouseY = event.mouseWheelScroll.y;
                
                set_hard();
            }
        }
        
        int32_t priority = focus.size() - 1;
        Capture captured = Capture::pass;

        if(hardFocus != nullptr) captured = std::max(captured, hardFocus->on_event(event, -1));
        
        for(Frame *frame : focus){
            if(captured == Frame::Capture::capture) break;
            captured = std::max(captured, frame->on_event(event, priority));
            priority--;
        }
    
    }
}

void Window::loop(){
    
    Clock clock(1.0 / 60);

    while(!destroyed){
        event_update();
        if(!destroyed) tick();
        if(!destroyed) window_refresh();
        clock.join_tick();
    }

    kill_children();
}

void Window::window_refresh(){
    
    // if(!refreshFlag) return;
    
    if(!resizeFlag){
        previousFrame.update(*this);
        previousFrameSprite.setTexture(previousFrame, 1);
    }

    clear(sf::Color::Transparent);
    
    if(!resizeFlag) draw(previousFrameSprite);

    resizeFlag = 0;

    refresh();
    
    display();
}

std::array<float, 2> Window::mouse(){ return {mouseX, mouseY}; }

Frame *Window::get_soft_focus(){ return softFocus; }

Frame *Window::get_hard_focus(){ return hardFocus; }

void Window::on_close(){}

void Window::on_resize(){}

void Window::attach_child(Window *child){
    children.insert(child);
}

void Window::kill_child(Window *child){
    if(children.count(child)){
        children.erase(child);
        delete child;
    }
}

void Window::kill_children(){
    for(auto child : children){
        child->kill_children();
        delete child;
    }
}

void Window::style(){
    update_style();
    for(auto child : children) child->style();
}

void Window::set_hard(){

    focus = find_focus();
    softFocus = focus.back();
    
    hardFocus = nullptr;
    for(int32_t i=focus.size() - 1; i > 0; i--){
        if(focus[i]->focusable){
            hardFocus = focus[i];
            break;
        }
    }
}

}
