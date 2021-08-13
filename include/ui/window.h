#ifndef UI_WINDOW_H
#define UI_WINDOW_H

namespace ui{
class Window;
}

#include "ui/core.h"
#include "ui/app.h"
#include "ui/frame.h"

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include <SFML/Window.hpp>

namespace ui{

class Window{

    
    // Class built around sf::RenderWindow.
    

protected:

    App *app;
    Core *core;
    Frame *mainframe;
    sf::RenderWindow window;

    std::string title = "";

    // textures that are sent to be rendered on to this window.
    std::vector<TextureFrame> textures;

    float height = 10, width = 10;
    
    // frame with forced focus
    Frame *focused = nullptr;

public:
   
    // mouse position
    float mwpos = 0, mhpos = 0;

    Window();
    Window(App *app_, Core *core_, Frame *mainframe, std::map<std::string, std::string> values);
    
    // use key-value pairs as : {{"variable name", "value"}, {"k2", "v2"}}
    bool setup(std::map<std::string, std::string> values);

    // function to collect the textures created by the frames.
    bool send_texture(TextureFrame &texture);

    // updates.
    bool coreapp_update();
    bool listen_events();
    
    // Redraw & render.
    bool refresh();

};

}

#endif
