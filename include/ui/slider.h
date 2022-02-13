#pragma once

#include "ui/stack.h"
#include "ui/text.h"

#include <SFML/Graphics/CircleShape.hpp>

namespace ui {

class Slider : public Frame {

    /*
       style:
       
       borderColor (color)
       borderThickness (num) or (num(left) num(right) num(up) num(down))
       background (color)
       
       barSize (num)
       
       dotSize (num)
       dotBorderThickness (num)
       dotColor (color)
       dotBorderColor (color)
       
       textSize (num)
       textColor (color)
       textStyle (textStyle)
       font (font)
     */

public:

    Slider(Window *master_, Side barSide, Side stackStick, Kwargs = {});
    virtual ~Slider();
    
    virtual void set_look(std::string look_);

    // action when slider is resized by dragging the bar.
    virtual void on_slide(sf::Mouse::Button, float width, float height);
    virtual void on_scroll(float delta);

    void set_scrollable(bool);
    void set_slidable(bool);

    void set_label(std::string);
    std::string get_label();

    Stack stack;
    
    // overload the stack with some other content.
    void overload_inner(Frame *content);

protected:
    
    class Bar : public Frame {

    public:
        
        Bar(Window*, Slider*, Side);

        Capture on_event(sf::Event event, int priority);

        void set_label(std::string);
        std::string get_label();
        
        void set_scrollable(bool);
        void set_slidable(bool);

    private:

        float directionX = 0, directionY = 0;

        std::array<float, 2> leftBegin, rightBegin;
        std::array<float, 2> leftDrag, rightDrag;
        bool leftPressed = 0, rightPressed = 0;

        bool scrollable = 1, slidable = 1;
        
        Slider *slider;
    };

    class Dot : public Frame {
    
    public:
   
        Dot(Window *master_, Kwargs = {});

        void set_look(std::string look_);
        void on_reconfig();
        void on_refresh();

        void fill_by_border(bool);

    private:

        sf::CircleShape dot;

    };

    Stack buttons;
    Frame functions;
    
    Bar bar;
    Dot dot;
    Text label;
    
    Side side;

};

}
