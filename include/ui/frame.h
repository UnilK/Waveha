#pragma once

namespace ui{
class TextureFrame;
class Frame;
}

#include "ui/core.h"
#include "ui/window.h"

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <map>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

namespace ui{

class TextureFrame{

    // utility class for passing around textures

public:

    // texture and it's offset position. wwpos, whpos is the texture's
    // global position. wpos, hpos is the left upper corner of the
    // remaining window on the texture. width, height are the
    // dimensions of the remaining window.

    const sf::Texture *tex;
    float wwpos, whpos, wpos, hpos, width, height;

    TextureFrame(const sf::Texture *tex_, float wwpos_, float whpos_);
};

class Frame{

    /*
       Class for managing input focus and ui layout.
       Pretty much everything else UI-related is
       built around this class.
       
       Frames are structured in a tree-like fashion,
       with the core and window (master frame) linked to every frame.

    */

protected:

    Core *core;
    Frame *parent;
    Window *master;

    std::string look = "";

    /*
        height & width and window height & window width:

        ##############
        #            #
        #  ######    #
        #  #    #    #
        #  #    #    #
        #  #    #    #
        #  #    #    #
        #  ######    #
        #            # 
        ##############

        A large canvas (cwidth, cheight) is viewed through a small
        window (window width, window height)
        
        The dimensions of the large canvas must be larger than or
        equal to the dimensions of the window i.e. the canvas is
        responsible for using ALL the space that the window provides.
        The canvas is also responsible for providing enough space for
        all of it's child frames. However, the target width & height
        are used for determining how much space a frame needs in it's
        parent's window.

        Rubber band analogy. There are 3 steps to putting a rubber
        around your wrist:

        1. Get a rubber band (target dimensions)
        2. Strecth it so that it fits through your hand (canvas dimensions)
        3. Snap it around you wrist (window dimensions)

        Dimension directions on the grid are:
        
          0 1 2 +
        0 # # #
        1 # # #
        2 # # #
        +

    */

    // the actual window
    sf::RenderTexture *window;
    
    // canvas dimensions & coordinates of the upper left corner of the canvas
    // with respect to the upper left corner of the window.
    float cwidth = 0, cheight = 0;
    float cwpos = 0, chpos = 0;
    
    // canvas target dimensions
    float twidth = 0, theight = 0;

    // window dimensions & global position of the window in the main frame.
    float wwidth = 0, wheight = 0;
    float wwpos = 0, whpos = 0;

    // grid layout management
    int32_t columns = 0, rows = 0;
    std::vector<float> wfill, hfill, wmax, hmax;
    std::vector<std::vector<Frame*> > grid;

    // padding & alignment
    float lpad = 0, rpad = 0, upad = 0, dpad = 0;
    float lalign = 0, ralign = 0, ualign = 0, dalign = 0;

    // textures that are sent to be rendered on to this frame.
    std::vector<TextureFrame> textures;

public:

    // this frame claims this much space in it's parent's grid.
    int32_t rowSpan = 1, columnSpan = 1; 
    
    bool refreshFlag = 1;

    Frame();
    Frame(Window *master_, std::map<std::string, std::string> values = {});
    Frame(Frame *parent_, std::map<std::string, std::string> values = {});
    ~Frame();

    // use key-value pairs as : {{"variable name", "value"}, {"k2", "v2"}}
    int32_t setup(std::map<std::string, std::string> values);

    // updates. Overload these if needed
    virtual int32_t event_update(sf::Event);
    virtual int32_t coreapp_update();

    // find the frame where the cursor is.
    Frame *find_focus();
    
    // send a texture to the next frame up that has the refreshFlag set.
    int32_t send_texture(TextureFrame &tex);

    // Redraw & render.
    int32_t refresh();

    // set the window size
    int32_t set_window_size(float wwidth_, float wheight_);
    int32_t set_window_position(float wwpos_, float whpos_);

    // set and get canvas target dimensions. padding is included
    // int the target dimensions.
    int32_t set_target_dimensions(float twidth_, float theight_);
    float target_width();
    float target_height();
    
    // setup a nullptr grid of size {rows_, columns_}
    int32_t setup_grid(int32_t rows_, int32_t columns_);

    // update window sizes in the grid. Call this with
    // direction = 1 on mainframe when initializing a window.
    // Directions in the frame tree: 0=stop, 1=down, 2=up, 3=both
    int32_t update_grid(int32_t direction = 0);

    // configure extra space allocation among rows & columns.
    // each row gets value[row]/values_sum extra space allocated to it.
    // if none of the rows or columns use the extra then
    // frames stick to the top left corner.
    int32_t config_row(std::vector<float> rfill_);
    int32_t config_column(std::vector<float> cfill_);
    int32_t config_row(int32_t row, float value);
    int32_t config_column(int32_t column, float value);

    // set padding around the frame contents
    int32_t pad(float left, float right, float up, float down);

    // by default the frame sticks to the lower left corner.
    // align the frame contents. examples for cases where the frame
    // itself does not use any of the extra space it is allocated:
    // align(1, 1, 1, 1) -> stay in the middle
    // align(0, 1, 0, 1) -> stick to the upper left corner
    // align(1, 2, 3, 0.5) -> stick third way from left and seventh
    //                        way from bottom.
    int32_t align(float left, float right, float up, float down);
    

};

}

