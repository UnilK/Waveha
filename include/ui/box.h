#pragma once

#include "ui/slider.h"
#include "ui/button.h"

namespace ui {

class Box : public Slider {

    /*
       a slider which assumes that it is gridded in a ui::Stack.

       style:
       
       buttons (ui::Button look)

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

    Box(Window *master_, Side barSide, Side stackStick, Kwargs = {});

    virtual void set_look(std::string look_);
    
    void on_slide(sf::Mouse::Button, float width, float height);
    
    // set the manager stack
    Box *set_manager(Stack*);

protected:

    void func_up();
    void func_down();
    void func_click();
    void func_maximize();
    void func_minimize();
    virtual void func_detach();

private:

    Stack *manager = nullptr;

    Button pushUp, pushDown, click, maximize, minimize, detach;

};

}

