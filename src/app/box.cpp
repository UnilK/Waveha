#include "app/box.h"

#include <iostream>

namespace app {

Box::Box(ui::Window *master_, std::string title_, Kwargs kwargs) :
    ResizableFrame(master_, {0, 0, 0, 15}, {0, 0, 0, 1}, {.look = "Box", .height = 200}),
    inner(master_),
    titleText(master_, title_, {.look = "BoxTitle", .height = 20}),
    buttonFrame(master_, {.look = "ButtonBox", .height = 24}),
    upSwitch(master_, this, "up", {.look = "BoxLeftButton", .width = 50, .height = 20}),
    downSwitch(master_, this, "down", {.look = "BoxLeftButton", .width = 50, .height = 20}),
    detachButton(master_, this, "detach", {.look = "BoxRightButton", .width = 70, .height = 20})
{
   
    title = title_;

    set_inner(&inner);

    inner.setup_grid(2, 1);
    inner.put(0, 0, &buttonFrame);

    inner.fill_width({1});
    inner.fill_height({0, 1});

    buttonFrame.setup_grid(1, 10);
    buttonFrame.fill_height(0, 1);
    
    buttonFrame.fill_width(5, 1);

    buttonFrame.put(0, 0, &upSwitch);
    buttonFrame.put(0, 1, &downSwitch);
    buttonFrame.put(0, 5, &titleText);
    buttonFrame.put(0, 9, &detachButton);

}

bool Box::place_to_tab(Tab *newTab){
    detach_from_tab();
    bool ok = newTab->add_box(this);
    if(!ok) return 0;
    tab = newTab;
    return 1;
}

void Box::detach_from_tab(){
    if(tab != nullptr){
        tab->detach_box(index());
        tab = nullptr;
    }
}

Tab *Box::get_tab(){ return tab; }

void Box::rename(std::string name){
    title = name;
    titleText.set_text(title);
}

std::string Box::get_title(){ return title; }

void Box::BU::function(){
    Box &x = *(Box*)commander;
    x.tab->swap_box(x.index(), -1);
}

void Box::BD::function(){
    Box &x = *(Box*)commander;
    x.tab->swap_box(x.index(), 1);
}

void Box::BX::function(){
    Box &x = *(Box*)commander;
    x.detach_from_tab();
}

int32_t Box::index(){
    if(tab == nullptr) return -1;
    return tab->box_index(this);
}

}
