#pragma once

namespace ui { class Frame; }

#include "ui/core.h"
#include "ui/window.h"
#include "ui/borders.h"
#include "ui/style.h"

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

namespace ui {

struct FrameArgs {

    // proxy class for kwargs-like initialization.

    std::string look = "";
    float width = 0;
    float height = 0;
    uint32_t spanx = 1;
    uint32_t spany = 1;
    std::array<float, 4> pad = {0, 0, 0, 0}; // left, right, up, down
    std::array<float, 6> fill = {0, 1, 0, 0, 1, 0}; // left, mid, right, up, mid, down.

};

typedef const FrameArgs& Kwargs;

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

       Child frame space allocation protocol:

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

       In content frames (frames that actually draw stuff) [Content] = [Canvas]
       
       If the Frame needs to stretch to fit
       the window, then [Window] = [Canvas]
       
       If nothing needs to be squished or pushed out of sight, then
       [Window] = [Canvas] = [Content]


       Dimension directions on the grid are:
       
         0 1 2 +
       0 # # #
       1 # # #
       2 # # #
       +
    */

public:

    Frame(Window *master_, Kwargs = {});
    virtual ~Frame();

    // events and actions.

    // priority 0 is for the inner most focused frame. the second
    // inner most frame will get priority 1 and so on...
    // hard focus event gets priority -1.
    enum Capture { pass, use, capture };
    virtual Capture on_event(sf::Event event, int32_t priority);
    
    // override void set_look(std::string) inherited from Styled for look updates

    // tick and actions related to a tick. 1 tick = 1 frame displayed. IsIs raan before refresh.
    void tick();
    virtual void on_tick();

    // actions run when window is resized/moved. Is ran before rerefresh.
    virtual void on_reconfig();
    
    // draw contents on the master window.
    void refresh();
    virtual void on_refresh();
    
    // inform the frame that it should refresh itself.
    void set_refresh();

    // mouse utility functions.
    std::array<float, 2> global_mouse();
    std::array<float, 2> local_mouse();
    bool contains_mouse();
    
    // find focus chain. the outer most frame comes first,
    std::vector<Frame*> find_focus();



    // window management

    // set the window size & positions
    void set_window_size(float windowWidth_, float windowHeight_);
    void set_window_position(float windowX_, float windowY_);
    void set_global_position(float globalX_, float globalY_);

    // set canvas size and position.
    void set_canvas_size(float targetWidth_, float targetHeight_);
    void set_canvas_position(float canvasX_, float canvasY_);
    void move_canvas(float deltaX, float deltaY);

    // set target canvas size
    void set_target_size(float targetWidth_, float targetHeight_);

    // get Frame size with padding included
    float target_width();
    float target_height();

    // automatically contain window in canvas. doesn't update grid.
    void auto_contain(bool);
    


    // grid management

    // setup a nullptr grid of size {rows_, columns_}
    void setup_grid(uint32_t rows_, uint32_t columns_);

    // resize grid
    void resize_grid(uint32_t rows_, uint32_t columns_);

    // update window and canvas sizes in the grid. If the frame has a grid
    // this needs to be called if window, target or canvas position
    // or size is changed. Sets refreshFlag.
    void update_grid();
    
    // parent management
    void set_parent(Frame *parent_);
    Frame *get_parent(int32_t steps = 1);
    Frame *get_top();

    // place frames to the grid. Use these outside initialization.
    int32_t place_frame(uint32_t row, uint32_t column, Frame *frame);
    int32_t remove_frame(uint32_t row, uint32_t column);
    int32_t remove_frame(Frame *frame);
    
    // method for placing frames to the grid without updating the grid.
    // use this for initialization.
    int32_t put(uint32_t row, uint32_t column, Frame *frame);
    
    // access the grid
    Frame *&get(uint32_t row, uint32_t column);
    Frame *&get(Frame *frame);

    // configure extra space allocation among rows & columns.
    // each row gets value[row]/values_sum extra space allocated to it.
    // if none of the rows or columns use the extra then
    // frames stick to the top left corner.
    void fill_height(std::vector<float> heightFill_);
    void fill_width(std::vector<float> widthFill_);
    void fill_height(uint32_t row, float value);
    void fill_width(uint32_t column, float value);

    void set_span(uint32_t rowSpan_, uint32_t columnSpan_);



    // cosmetics
    
    void pad(float left, float right, float up, float down);
    void fill(float left, float midh, float right, float up, float midv, float down);

    // update all looks
    void update_style();



    // getters...
    // giving each variable below a getter wouldn't have much benefits. Use only for access.

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
    
protected:
    
    Window *master = nullptr;
    Frame *parent = nullptr;
    Core *core = nullptr;
    
private:

    // grid and fill configuration
    std::vector<float> widthFill, heightFill;
    std::vector<std::vector<Frame*> > grid;
    uint32_t columns = 0, rows = 0;

    // grid content layout management
    std::vector<float> widthMax = {0}, heightMax = {0};

    // The frame's size on it's parent's grid.
    uint32_t rowSpan = 1, columnSpan = 1; 
    
    // Alignment and padding.
    float frameFillWidth = 1, frameFillHeight = 1;
    float alignPadLeft = 0, alignPadRight = 0, alignPadDown = 0, alignPadUp = 0;
    float alignFillLeft = 0, alignFillRight = 0, alignFillDown = 0, alignFillUp = 0;

    // flags
    bool refreshFlag = 1;
    bool reconfigFlag = 0;

    // is the window size 0?
    bool degenerate();
    
    // find focus
    void find_focus_inner(std::vector<Frame*> &focus);
    
    bool autoContain = 1;

};



class SolidFrame : public Frame {
 
    /*
       style parameters:
       borderColor (color)
       borderThickness (num) or (num(left) num(right) num(up) num(down))
       background (color)
    */

public:

    SolidFrame(Window *master_, Kwargs = {});
    virtual ~SolidFrame();

    void set_look(std::string look_);
    void on_reconfig();
    void on_refresh();

private:

    Borders border;

};

}

