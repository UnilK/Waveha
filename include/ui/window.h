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
        sf::RenderWindow *window;

        std::string title = "";

        // textures that are sent to be rendered on to this window.
        std::vector<TextureFrame> textures;

        float height = 100, width = 100;

        friend class Frame;

    public:
       
        // mouse position
        float mouseX = 0, mouseY = 0;

        Window();
        Window(Core *core_, std::map<std::string, std::string> values = {});

        // delete the window and set the destroyed flag.
        int32_t destroy();
        bool destroyed = 0;

        // use key-value pairs as : {{"variable name", "value"}, {"k2", "v2"}}
        int32_t setup(std::map<std::string, std::string> values);

        // function to collect the textures created by the frames.
        int32_t send_texture(TextureFrame &texture);

        // updates.
        int32_t coreapp_update();
        int32_t listen_events();

        // overloadable responses to events.
        virtual int32_t on_close(sf::Event event);
        virtual int32_t on_resize(sf::Event event);
        virtual int32_t on_mouse_move(sf::Event event);
        
        // Redraw & render.
        int32_t refresh();

    };

}

