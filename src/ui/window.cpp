#include "ui/window.h"

#include <iostream>
#include <sstream>
#include <math.h>

namespace ui{

Window::Window(Core *core_, float width_, float height_,
        std::string title_, long double refreshRate) :
    sf::RenderWindow(sf::VideoMode(width_, height_), title_),
    clock(1.0l/refreshRate)
{
    core = core_;
    width = width_;
    height = height_;
    title = title_;
    core->add_window(this);
}

int32_t Window::destroy(){
    if(destroyed) return 1;
    destroyed = 1;
    for(Window *child : children) child->destroy();
    return 0;
}

int32_t Window::tick_update(){
    if(destroyed) return 0;
    mainframe->tick_update();
    return 0;
}

int32_t Window::event_update(){
    
    if(destroyed) return 0;

    sf::Event event;
    while (pollEvent(event)){
        switch(event.type){
            case sf::Event::Closed:
                
                close();
                on_close();
                
                return destroy();

            case sf::Event::Resized:
                
                width = event.size.width;
                height = event.size.height;
				
                setView(sf::View(sf::FloatRect(0.0f, 0.0f, width, height)));
                
                mainframe->set_window_size(width, height);
                mainframe->set_canvas_size(width, height);
                mainframe->update_grid();
                
                on_resize();
                
                break;
            case sf::Event::MouseMoved:
                
                mouseX = event.mouseMove.x;
                mouseY = event.mouseMove.y;
                soft_focus = mainframe->find_focus();
                on_mouse_move();
                
                break;
            default:

                if(event.type == sf::Event::MouseButtonPressed){
                    clicked = soft_focus;
                    if(soft_focus != nullptr) soft_focus->event_update(event);
                }
                else if(event.type == sf::Event::MouseButtonReleased){
                    if(soft_focus == clicked && clicked != nullptr && clicked->focusable)
                        hard_focus = clicked;
                    if(soft_focus != clicked)
                        clicked->event_update(event);
                    if(soft_focus != nullptr)
                        soft_focus->event_update(event);
                }
                else if(hard_focus != nullptr)
                    hard_focus->event_update(event);
                
                break;
        }
    }


    return 0;
}

int32_t Window::refresh(){
    
    if(destroyed) return 0;

    mainframe->refresh();
    if(displayFlag) display();
    displayFlag = 0;

    return 0;
}

Frame *Window::get_soft_focus(){ return soft_focus; }

Frame *Window::get_hard_focus(){ return hard_focus; }

Frame *Window::get_clicked(){ return clicked; }

Core *Window::get_core(){ return core; };

int32_t Window::on_close(){ return 0; }

int32_t Window::on_resize(){ return 0;  }

int32_t Window::on_mouse_move(){ return 0; }

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

}
