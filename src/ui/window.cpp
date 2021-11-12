#include "ui/window.h"

#include <iostream>
#include <sstream>
#include <math.h>

namespace ui{

Window::Window(Core *core_, float width_, float height_, std::string title_) :
    sf::RenderWindow(sf::VideoMode(width_, height_), title_)
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
    return 0;
}

int32_t Window::coreapp_update(){
    if(destroyed) return 0;
    mainframe->coreapp_update();
    return 0;
}

int32_t Window::listen_events(){
    
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
                on_mouse_move();
                
                break;
            default:
                
                mainframe->event_update(event);
                break;
        }
    }


    return 0;
}

/*
void Window::draw(
        const sf::Drawable &drawable,
        const sf::RenderStates &states){
    window.draw(drawable, states);
}

void Window::draw(
        const sf::Vertex *vertices,
        std::size_t vertexCount,
        sf::PrimitiveType type,
        const sf::RenderStates &states){
    window.draw(vertices, vertexCount, type, states);
}

void Window::draw(
        const sf::VertexBuffer &vertexBuffer,
        const sf::RenderStates &states){
    window.draw(vertexBuffer, states);
}

void Window::draw(
        const sf::VertexBuffer &vertexBuffer,
        std::size_t firstVertex,
        std::size_t vertexCount,
        const sf::RenderStates &states){
    window.draw(vertexBuffer, firstVertex, vertexCount, states);
}
*/

int32_t Window::refresh(){
    
    if(destroyed) return 0;

    mainframe->refresh();
    if(displayFlag) display();
    displayFlag = 0;

    return 0;
}

int32_t Window::on_close(){ return 0; }

int32_t Window::on_resize(){ return 0;  }

int32_t Window::on_mouse_move(){ return 0; }

}
