#include "ui/window.h"

#include <iostream>
#include <sstream>
#include <math.h>

namespace ui{

Window::Window(Core *core_, float width_, float height_,
        std::string title_, long double refreshRate) :
    sf::RenderWindow(sf::VideoMode(width_, height_), title_),
    clock(1.0f / refreshRate)
{
    core = core_;
    width = width_;
    height = height_;
    title = title_;
    core->add_window(this);

    previousFrame.create(width, height);
    
    setVerticalSyncEnabled(1);

}

int32_t Window::destroy(){
    if(destroyed) return 1;
    destroyed = 1;
    for(Window *child : children) child->destroy();
    return 0;
}

int32_t Window::core_tick(){
    on_core_tick();
    if(clock.try_tick()) window_tick();
    mainframe->core_tick();
    return 0;
}

int32_t Window::window_tick(){
    on_window_tick();
    mainframe->window_tick();
    refresh();
    return 0;
}

int32_t Window::on_core_tick(){ return 0; }

int32_t Window::on_window_tick(){ return 0; }

int32_t Window::event_update(){
    
    if(destroyed) return 0;

    sf::Event event;
    while (pollEvent(event)){
        
        if(event.type == sf::Event::Closed){
                
                close();
                on_close();
                
                return destroy();

        } else if(event.type == sf::Event::Resized){
                
                width = event.size.width;
                height = event.size.height;
				
                setView(sf::View(sf::FloatRect(0.0f, 0.0f, width, height)));
                previousFrame.create(width, height);

                mainframe->set_window_size(width, height);
                mainframe->set_canvas_size(width, height);
                mainframe->update_grid();

                resizeFlag = 1;
                
                on_resize();
                
        } else if(event.type == sf::Event::MouseMoved){
                
                mouseX = event.mouseMove.x;
                mouseY = event.mouseMove.y;

                focus = mainframe->find_focus();
                softFocus = focus.back();
                
        } else {

                if(event.type == sf::Event::MouseButtonPressed){
                    focus = mainframe->find_focus();
                    softFocus = focus.back();
                    hardFocus = softFocus;
                }
                
                if(event.type == sf::Event::MouseWheelScrolled){
                    focus = mainframe->find_focus();
                    softFocus = focus.back();
                    hardFocus = softFocus;
                }
        }
        
        int32_t priority = focus.size() - 1;
        int32_t captured = 0;

        if(hardFocus != nullptr) captured = hardFocus->on_event(event, -1);
        
        for(Frame *frame : focus){
            if(captured > 0) break;
            if(frame != hardFocus) captured = std::max(captured, frame->on_event(event, priority));
            priority--;
        }
    
    }

    return 0;
}

int32_t Window::refresh(){
    
    if(destroyed) return 0;
    if(!refreshFlag) return 0;

    if(!resizeFlag){
        previousFrame.update(*this);
        previousFrameSprite.setTexture(previousFrame, 1);
    }

    clear(sf::Color::Transparent);
    
    if(!resizeFlag){
        draw(previousFrameSprite);
    }

    resizeFlag = 0;

    mainframe->refresh();
  
    display();
   
    refreshFlag = 0;

    return 0;
}

void Window::set_refresh(){
    refreshFlag = 1;
}

Frame *Window::get_soft_focus(){ return softFocus; }

Frame *Window::get_hard_focus(){ return hardFocus; }

Core *Window::get_core(){ return core; };

int32_t Window::on_close(){ return 0; }

int32_t Window::on_resize(){ return 0; }

int32_t Window::attach_child(Window *child){
    if(children.count(child)) return 1;
    children.insert(child);
    return 0;
}

int32_t Window::detach_child(Window *child){
    if(!children.count(child)) return 1;
    children.erase(child);
    return 0;
}

void Window::update_style(){
    mainframe->update_style();    
}

}
