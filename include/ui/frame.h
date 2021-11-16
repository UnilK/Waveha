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
#include <SFML/Graphics/RectangleShape.hpp>

namespace ui{

class Frame{

    /*
       Class for managing input focus and ui layout.
       Pretty much everything else UI-related is
       built around this class.
       
       Frames are structured in a tree-like fashion,
       with the core and window (master frame) linked to every frame.

    */

protected:

    Core *core = nullptr;
    Frame *parent = nullptr;
    Window *master = nullptr;

    std::string id = "";
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

        These allow:

        1. Intuitive scaling (1)
        2. Easy drawing of content frames (2)
        3. Efficiency (1, 3)

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

    // canvas dimensions.
    float targetWidth = 0, targetHeight = 0;
    float canvasWidth = 0, canvasHeight = 0;

    // window dimensions & window positions
    float windowWidth = 0, windowHeight = 0;
    float windowX = 0, windowY = 0;
    float globalX = 0, globalY = 0;

    // grid layout management
    int32_t columns = 0, rows = 0;
    std::vector<float> widthFill, heightFill;
    std::vector<std::vector<Frame*> > grid;

    // TODO implement off-grid frames

    // grid content layout management
    std::vector<float> widthMax = {0}, heightMax = {0};
    
    // frame content displacement: if the children of the frame require
    // more space than it has to offer, then this is the canvas' position
    // on the contents.
    float canvasX = 0, canvasY = 0;

    // Protected variables below this line should only be accessed by the parent frame.

    // The frame's size on it's parent's grid.
    int32_t rowSpan = 1, columnSpan = 1; 
    
    // Alignment and bordering.
    float frameFillWidth = 1, frameFillHeight = 1;
    float alignPadLeft = 0, alignPadRight = 0, alignPadDown = 0, alignPadUp = 0;
    float alignFillLeft = 0, alignFillRight = 0, alignFillDown = 0, alignFillUp = 0;

    bool read_value(std::string key, std::stringstream &value,
            std::map<std::string, std::string> &values);

private:

    int32_t setup(std::map<std::string, std::string> &values);

public:

    Frame(Window *master_, std::map<std::string, std::string> values = {});
    Frame(Frame *parent_, std::map<std::string, std::string> values = {});

    // is the window size 0?
    bool degenerate();

    // updates. Overload these if needed
    virtual int32_t event_update(sf::Event event);
    virtual int32_t coreapp_update();

    // draw contents and display them on the window
    virtual int32_t draw();

    // determine mouse position
    std::array<float, 2> global_mouse();
    std::array<float, 2> local_mouse();

    // find the frame where the cursor is.
    Frame *find_focus();
    bool focusable = 1;
    
    // Redraw & render.
    int32_t refresh();
    bool refreshFlag = 1;

    // set the window size & positions
    int32_t set_window_size(float windowWidth_, float windowHeight_);
    int32_t set_window_position(float windowX_, float windowY_);
    int32_t set_global_position(float globalX_, float globalY_);

    // set canvas size and position.
    int32_t set_canvas_size(float targetWidth_, float targetHeight_);
    int32_t set_canvas_position(float canvasX_, float canvasY_);
    int32_t move_canvas(float deltaX, float deltaY);

    // set target canvas size
    int32_t set_target_size(float targetWidth_, float targetHeight_);

    // padding & border included
    float target_width();
    float target_height();
    
    // setup a nullptr grid of size {rows_, columns_}
    int32_t setup_grid(int32_t rows_, int32_t columns_);

    // update window and canvas sizes in the grid.
    // Needs to be called if window, target or canvas position
    // or size is changed.
    int32_t update_grid();

    // configure extra space allocation among rows & columns.
    // each row gets value[row]/values_sum extra space allocated to it.
    // if none of the rows or columns use the extra then
    // frames stick to the top left corner.
    int32_t fill_height(std::vector<float> heightFill_);
    int32_t fill_width(std::vector<float> widthFill_);
    int32_t fill_height(int32_t row, float value);
    int32_t fill_width(int32_t column, float value);

    // alignment and bordering
    int32_t set_span(int32_t rowSpan_, int32_t columnSpan_);
    int32_t align(float left, float right, float up, float down);
    int32_t align_fill(float left, float right, float up, float down);
    int32_t frame_fill(float width, float height);
};



class SolidFrame : public Frame {
 
protected:

    // frame with a background color
    sf::RectangleShape background;

public:

    SolidFrame(Window *master_, std::map<std::string, std::string> values = {});
    SolidFrame(Frame *parent_, std::map<std::string, std::string> values = {});

    int32_t draw();
};



class ContentFrame : public Frame {
 
protected:

    // frame with an independent canvas to draw on
    sf::RenderTexture canvas;

public:

    ContentFrame(Window *master_, std::map<std::string, std::string> values = {});
    ContentFrame(Frame *parent_, std::map<std::string, std::string> values = {});

    int32_t initialize();
    int32_t display();
};

}

