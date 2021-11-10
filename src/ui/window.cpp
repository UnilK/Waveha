#include "ui/window.h"

#include <iostream>
#include <sstream>
#include <math.h>

namespace ui{

Window::Window(){}

Window::Window(Core *core_, std::map<std::string, std::string> values){
    core = core_;
    setup(values); 
    window = new sf::RenderWindow(sf::VideoMode(width, height), title);
    core->add_window(this);
}

int32_t Window::destroy(){
    if(destroyed) return 1;
    delete window;
    destroyed = 1;
    return 0;
}

int32_t Window::setup(std::map<std::string, std::string> values){
    if(destroyed) return 1;
    if(values["title"] != "") title = values["title"];
    if(values["width"] != "") std::stringstream(values["width"]) >> width;
    if(values["height"] != "") std::stringstream(values["height"]) >> height;
    return 0;
}

int32_t Window::send_texture(TextureFrame &texture){
    if(destroyed) return 0;
    textures.push_back(texture); 
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
    while (window->pollEvent(event)){
        switch(event.type){
            case sf::Event::Closed:
                window->close();
                on_close(event);
                return destroy();
            case sf::Event::Resized:
                width = event.size.width;
                height = event.size.height;
				window->setView(sf::View(sf::FloatRect(0.0f, 0.0f, width, height)));
                mainframe->set_window_size(width, height);
                mainframe->update_grid(3);
                on_resize(event);
                break;
            case sf::Event::MouseMoved:
                mouseX = event.mouseMove.x;
                mouseY = event.mouseMove.y;
                on_mouse_move(event);
                break;
            default:
                mainframe->event_update(event);
                break;
        }
    }


    return 0;
}

int32_t Window::refresh(){
    
    if(destroyed) return 0;

    mainframe->refresh();
    
    for(TextureFrame &tex : textures){
        sf::Sprite sprite(*tex.tex);
        float texHeight = tex.tex->getSize().y;
        sprite.setPosition(tex.x, height-tex.y-texHeight);
        window->draw(sprite);
    }

    if(!textures.empty()) window->display();
    
    textures.clear();

    return 0;
}

int32_t Window::on_close(sf::Event event){ return 0; }

int32_t Window::on_resize(sf::Event event){ return 0;  }

int32_t Window::on_mouse_move(sf::Event event){ return 0; }

}
