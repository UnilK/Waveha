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

class Frame;

class TextureFrame{

public:

    // utility class for passing textures to master window.
    const sf::Texture *tex;
    float x, y;

    TextureFrame(const sf::Texture *tex_, float x_, float y_);
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

        Top design principles:

        1. Top-to-Down authoritarian space allocation. When a parent frame
           order's it's child frames to use some amount of space, they
           must use it. Child frames also don't have any say in how much
           space they get.
        2. Alignment of any kind should be separate from frame
           contents. The frame contains only the frame, nothing else.
        3. All of the actual rendering is done on the master window.
        4. Borders are a part of the frame and drawn on top of the content.

        These allow:

        1. Intuitive scaling (1)
        2. Easy drawing of content frames (2)
        3. Efficiency (1, 3)
        4. Border to be drawn on the frame (4)

        The space allocation to child frames:

        1. Find the size of the window
        2. Find the size of the canvas, max(canvas size, window size)
        3. Determine it's position on the main frame
        4. Determine the window's position on the canvas
        5. Pass on recursively to chlid frames.

        ######################################
        #                                    #
        #   Content (space required by       #
        #            the child frames)       #
        #   ############################     #
        #   #   Canvas                 #     #
        #   #                          #     #
        #   #     ######################     #
        #   #     #                    #     #
        #   #     #  Window            #     #
        #   #     #   (this would be   #     #
        #   #     #    a top-left      #     #
        #   #     #    corner on the   #     #
        #   #     #    parent frame)   #     #
        #   ############################     #
        #                                    #
        ######################################

        In content frames [Content] = [Canvas]
        
        If the Frame needs to stretch to fit
        the window, then [Window] = [Canvas]
        
        If nothing needs to be squished or out of sight, then
        [Window] = [Canvas] = [Content]


        Dimension directions on the grid are:
        
          0 1 2 +
        0 # # #
        1 # # #
        2 # # #
        +

    */

    // the actual canvas
    sf::RenderTexture *canvas;
    
    // canvas default dimensions.
    float canvasWidth = 0, canvasHeight = 0;

    // window dimensions & window positions
    float windowWidth = 0, windowHeight = 0;
    float windowX = 0, windowY = 0;
    float globalX = 0, globalY = 0;

    // grid layout management
    int32_t columns = 0, rows = 0;
    std::vector<float> widthFill, heightFill;
    std::vector<std::vector<Frame*> > grid;

    // grid content layout management
    std::vector<float> widthMax, heightMax;
    
    // frame content displacement: if the children of the frame require
    // more space than it has to offer, then this is the canvas' position
    // on the contents.
    float canvasX = 0, canvasY = 0;

    // border thickness
    float borderLeft = 0, borderRight = 0, borderDown = 0, borderUp = 0;
    int32_t border(float left, float right, float up, float down);
    int32_t border(float thickness);

    // Protected variables below this line should only be accessed by the parent frame.

    // The frame's size on it's parent's grid.
    int32_t rowSpan = 1, columnSpan = 1; 
    
    // Alignment and bordering.
    float frameFillWidth = 1, frameFillHeight = 1;
    float alignPadLeft = 0, alignPadRight = 0, alignPadDown = 0, alignPadUp = 0;
    float alignFillLeft = 0, alignFillRight = 0, alignFillDown = 0, alignFillUp = 0;
    int32_t align(float left, float right, float up, float down);
    int32_t align_fill(float left, float right, float up, float down);
    int32_t frame_fill(float width, float height);

public:

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
    
    // Redraw & render.
    int32_t refresh();
    bool refreshFlag = 1;

    // set the window size & positions
    int32_t set_window_size(float windowWidth_, float windowHeight_);
    int32_t set_window_position(float windowX_, float windowY_);
    int32_t set_global_position(float globalX_, float globalY_);

    // set canvas target dimensions.
    int32_t set_canvas_size(float canvasWidth_, float canvasHeight_);
    
    // padding & border included
    float canvas_width();
    float canvas_height();
    
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
    int32_t fill_height(std::vector<float> heightFill_);
    int32_t fill_width(std::vector<float> widthFill_);
    int32_t fill_height(int32_t row, float value);
    int32_t fill_width(int32_t column, float value);

};

}

