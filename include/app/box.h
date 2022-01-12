#pragma once

namespace app { class Box; }

#include "ui/button.h"
#include "ui/text.h"

#include "app/resizableFrame.h"
#include "app/tab.h"

namespace app {

class Box : public ResizableFrame {

    /*
       style:
       -
       kwargs:
       -
     */

public:

    Box(ui::Window *master_, std::string title_, kwargs values = {});

    bool place_to_tab(Tab *newTab);
    void detach_from_tab();

    void rename(std::string name);

    std::string get_title();
    Tab *get_tab();

protected:

    ui::Frame inner;
    
    ui::Text titleText;
    
    Tab *tab = nullptr;
    int32_t index();

    std::string title;

private:
    
    ui::Frame buttonFrame;
    class BU : public ui::Button { using ui::Button::Button; void function(); }; BU upSwitch;
    class BD : public ui::Button { using ui::Button::Button; void function(); }; BD downSwitch;
    class BX : public ui::Button { using ui::Button::Button; void function(); }; BX detachButton;


};

}
