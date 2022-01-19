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

    Slider(Window *master_, Side barSide, Side stackStick, bool scrollable, Kwargs = {});
    virtual ~Slider();
    
    virtual void set_look(std::string look_);

    // action when slider is resized by dragging the bar.
    virtual void on_resize(sf::Mouse::Button, float width, float height);

    void set_label(std::string);
    std::string get_label();

    Stack stack;

protected:
    
    // overload the stack with some other content. use only on initialization.
    void overload_inner(Frame *content);
    
    class Bar : public Frame {

    public:
        
        Bar(Window *, Slider*, Stack*, Side, bool);

        Capture on_event(sf::Event event, int32_t priority);

        void set_label(std::string);
        std::string get_label();

    private:

        float directionX = 0, directionY = 0;

        std::array<float, 2> leftBegin, rightBegin;
        std::array<float, 2> leftDrag, rightDrag;
        bool leftPressed = 0, rightPressed = 0;

        bool scrollable = 0;
        
        Slider *slider;
        Stack *scrolled;
    };

    class Dot : public Frame {
    
    public:
   
        Dot(Window *master_, Kwargs = {});

        void set_look(std::string look_);
        void on_reconfig();
        void on_refresh();

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
