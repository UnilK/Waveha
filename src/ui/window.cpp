#include "ui/window.h"

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

Window::~Window(){ delete window; }

int32_t Window::destroy(){
    if(destroyed) return 1;
    destroyed = 1;
    return 0;
}

int32_t Window::setup(std::map<std::string, std::string> values){
    if(destroyed) return 1;
    
    std::map<std::string, int> index = {
        {"title", 1},
        {"width", 2},
        {"height", 3}
    };

    for(auto [k, v] : values){
        std::stringstream values(v);
        switch(index[k]){
            case 0:
                break;
            case 1:
                values >> title;
                break;
            case 2:
                values >> width;
                break;
            case 3:
                values >> height;
                break;
        }
    }

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
                on_close();
                return destroy();
            case sf::Event::Resized:
                width = event.size.width;
                height = event.size.height;
				window->setView(sf::View(sf::FloatRect(0.0f, 0.0f, width, height)));
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

int32_t Window::refresh(){
    
    if(destroyed) return 0;

    mainframe->refresh();
    
    for(TextureFrame &tex : textures){
        sf::IntRect area(tex.localX, tex.localY, tex.width, tex.height);
        sf::Sprite sprite(*tex.tex, area);
        sprite.setPosition(tex.globalX, tex.globalY);
        window->draw(sprite);
    }

    if(!textures.empty()) window->display();
    
    textures.clear();

    return 0;
}

int32_t Window::on_close(){ return 0; }

int32_t Window::on_resize(){ return 0;  }

int32_t Window::on_mouse_move(){ return 0; }

}
