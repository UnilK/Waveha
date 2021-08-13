#include "ui/window.h"

#include <sstream>
#include <math.h>

namespace ui{

Window::Window(){}

Window::Window(
        App *app_, Core *core_, Frame *mainframe_,
        std::map<std::string, std::string> values){
    app = app_;
    core = core_;
    mainframe = mainframe_;
    setup(values); 
    sf::RenderWindow window(sf::VideoMode(width, height), title);
}

bool Window::setup(std::map<std::string, std::string> values){
    if(values["title"] != "") title = values["title"];
    if(values["width"] != "") std::stringstream(values["width"]) >> width;
    if(values["height"] != "") std::stringstream(values["height"]) >> height;
    return 1;
}

bool Window::send_texture(TextureFrame &texture){
    textures.push_back(texture); 
    return 1;
}

bool Window::coreapp_update(){
    mainframe->coreapp_update();
    return 1;
}

bool Window::listen_events(){
    
    if(!window.isOpen()) return 0;

    sf::Event event;
    while (window.pollEvent(event)){
        switch(event.type){
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::Resized:
                width = event.size.width;
                height = event.size.height;
				window.setView(sf::View(sf::FloatRect(0.0f, 0.0f, width, height)));
                mainframe->wwidth = width;
                mainframe->wheight = height;
                mainframe->update_grid();
                break;
            case sf::Event::MouseMoved:
                mwpos = event.mouseMove.x;
                mhpos = event.mouseMove.y;
            default:
                mainframe->event_update(event);
                break;
        }
    }

    return 1;
}

bool Window::refresh(){
    
    if(!window.isOpen()) return 0;

    mainframe->refresh();
    
    for(TextureFrame &tex : textures){
        if(tex.height >= 0.5f && tex.width >= 0.5f){
            sf::Rect<int32_t> rect(
                    std::round(tex.wpos), std::round(tex.hpos),
                    std::round(tex.width), std::round(tex.height));
            sf::Sprite sprite(tex.tex, rect);
            sprite.setPosition(-tex.wwpos+tex.wpos, -tex.whpos+tex.hpos);
            window.draw(sprite);
        }
    }

    if(!textures.empty()) window.display();
    
    textures.clear();

    return 1;
}

}
