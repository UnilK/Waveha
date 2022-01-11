#pragma once

namespace ui { class Frame; }

#include "ui/core.h"
#include "ui/window.h"
#include "ui/borders.h"

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <map>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Font.hpp>

typedef std::map<std::string, std::string> kwargs;

namespace ui {

class Frame : public Styled {

    /*
       Class for managing input focus and ui layout.
       Pretty much everything else UI-related is
       built around this class.
       
       Frames are structured in a tree-like fashion,
       with the core and window (master frame) linked to every frame.

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

    /*
       style:
       -

       kwargs:
       
       look (chars)
       width (num)
       height (num)
       columnSpan (num)
       rowSpan (num)
       pad (num(left) num(right) num(up) num(down))
       fill (num(l) num(m) num(r) num(u) num(m) num(d))

    */

public:

    Frame(kwargs values = {});
    Frame(Window *master_, kwargs values = {});
    Frame(Frame *parent_, kwargs values = {});
    virtual ~Frame();

    void set_parent(Frame *parent_);

    // is the window size 0?
    bool degenerate();

    // events.
    // priority 0 is for the inner most focused frame. the second
    // inner most frame will get priority 1 and so on...
    // hard focus event gets priority -1.
    std::vector<Frame*> find_focus();
    void find_focus_inner(std::vector<Frame*> &focus);

    // on_event returns capture value.
    // -1 frame didn't use event
    // 0 frame used event but didn't capture it
    // 1 frame used event and captured it.
    virtual int32_t on_event(sf::Event event, int32_t priority = 0);
    
    // can this frame assume hard focus?
    bool focusable = 1;

    // tick and actions related to a tick. 1 tick = 1 frame displayed.
    int32_t tick();
    virtual int32_t on_tick();

    // draw contents and display them on the window
    virtual int32_t draw();

    // actions run when window is resized/moved
    virtual int32_t on_reconfig();

    // determine mouse position
    std::array<float, 2> global_mouse();
    std::array<float, 2> local_mouse();
    bool contains_mouse();
    
    // Redraw & render.
    int32_t refresh();

    // inform the frame that it should refresh itself.
    void set_refresh();

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
    std::array<float, 2> get_target_canvas_size();

    // padding & border included
    float target_width();
    float target_height();
    
    // setup a nullptr grid of size {rows_, columns_}
    int32_t setup_grid(int32_t rows_, int32_t columns_);

    // resize grid
    int32_t resize_grid(int32_t rows_, int32_t columns_);

    // update window and canvas sizes in the grid.
    // Needs to be called if window, target or canvas position
    // or size is changed. Sets refreshFlag.
    int32_t update_grid();

    // place frames to the grid.
    int32_t place_frame(int32_t row, int32_t column, Frame *frame);
    int32_t remove_frame(int32_t row, int32_t column);
    int32_t remove_frame(Frame *frame);
    
    // method for placing frames to the grid without updating the grid.
    // use this for initialization.
    int32_t put(int32_t row, int32_t column, Frame *frame);
    
    // access the grid
    Frame *&get(int32_t row, int32_t column);
    Frame *&get(Frame *frame);

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

    // get parent some steps up
    Frame *get_parent(int32_t steps = 1);
    Frame *get_top();

    // giving each of these variables getters would be stupid.
    // use only for access.

    // canvas target dimensions.
    float targetWidth = 0, targetHeight = 0;
    
    // canvas dimensions
    float canvasWidth = 0, canvasHeight = 0;
    
    // frame content displacement: if the children of the frame require
    // more space than it has to offer, then this is the canvas' position
    // on the contents.
    float canvasX = 0, canvasY = 0;

    // window dimensions & window positions
    float windowWidth = 0, windowHeight = 0;
    float windowX = 0, windowY = 0;
    float globalX = 0, globalY = 0;
    
    // grid dimensions
    int32_t columns = 0, rows = 0;
    
    // automatic view containment
    bool autoContain = 1;

    // update all looks
    void update_style();

protected:

    Core *core = nullptr;
    Frame *parent = nullptr;
    Window *master = nullptr;

    // grid and fill configuration
    std::vector<float> widthFill, heightFill;
    std::vector<std::vector<Frame*> > grid;

    // grid content layout management
    std::vector<float> widthMax = {0}, heightMax = {0};

    // The frame's size on it's parent's grid.
    int32_t rowSpan = 1, columnSpan = 1; 
    
    // Alignment and bordering.
    float frameFillWidth = 1, frameFillHeight = 1;
    float alignPadLeft = 0, alignPadRight = 0, alignPadDown = 0, alignPadUp = 0;
    float alignFillLeft = 0, alignFillRight = 0, alignFillDown = 0, alignFillUp = 0;

    // parser for poor man's **kwargs
    bool read_value(std::string key, std::stringstream &value, kwargs &values);

    // flags
    bool refreshFlag = 1;
    bool windowMoved = 0;
    bool windowResized = 0;
    bool canvasMoved = 0;
    bool canvasResized = 0;
    
    bool reconfig_check();
    void unset_reconfig();

private:

    int32_t setup(kwargs &values);

};



class SolidFrame : public Frame {
 
    /*
       style parameters:
       borderColor (color)
       borderThickness (num) or (num(left) num(right) num(up) num(down))
       background (color)
    */

public:

    SolidFrame(kwargs values = {});
    SolidFrame(Window *master_, kwargs values = {});
    SolidFrame(Frame *parent_ = nullptr, kwargs values = {});

    int32_t draw();
    int32_t on_reconfig();

    int32_t set_look(std::string look_);

protected:

    Borders border;

};



class ContentFrame : public Frame {
 
public:

    ContentFrame(kwargs values = {});
    ContentFrame(Window *master_, kwargs values = {});
    ContentFrame(Frame *parent_ = nullptr, kwargs values = {});

    int32_t on_reconfig();
    int32_t display();

    virtual int32_t inner_reconfig();

protected:

    sf::RenderTexture canvas;
    sf::Sprite canvasSprite;

};

}

