#pragma once

namespace ui{
class Window;
}

#include "ui/core.h"
#include "ui/frame.h"

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include <SFML/Graphics/RenderWindow.hpp>

namespace ui{

class Window{

    
    /*
       Class built around sf::RenderWindow.
       Create new instances of this class with "new",
       core will handle the memory.
    */

    protected:

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
        Window(Core *core_, Frame *mainframe, std::map<std::string, std::string> values = {});

        // destroy window by removing it from the core.
        int32_t destroy();
        
        // use key-value pairs as : {{"variable name", "value"}, {"k2", "v2"}}
        int32_t setup(std::map<std::string, std::string> values);

        // function to collect the textures created by the frames.
        int32_t send_texture(TextureFrame &texture);

        // updates.
        int32_t coreapp_update();
        int32_t listen_events();
        
        // Redraw & render.
        int32_t refresh();

    };

}

