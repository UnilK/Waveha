#include "app/box.h"

#include <iostream>

namespace app {

Box::Box(ui::Window *master_, std::string title_, kwargs values) :
    ResizableFrame(master_, kwargs
            {{"look", "Box"},
            {"height", "200"},
            {"border", "0 0 0 15"},
            {"resize", "0 0 0 1"},
            {"pad", "0 0 0 0"}}),
    inner(master_),
    titleText(master_, kwargs
            {{"look", "BoxTitle"},
            {"height", "20"},
            {"text", title_}}),
    buttonFrame(master_, kwargs{{"look", "ButtonBox"}, {"height", "24"}}),
    upSwitch(master_, this, kwargs
            {{"text", "up"},
            {"height", "20"},
            {"width", "50"},
            {"look", "BoxLeftButton"},
            {"pad", "0 0 0 0"}}),
    downSwitch(master_, this, kwargs
            {{"text", "down"},
            {"height", "20"},
            {"width", "50"},
            {"look", "BoxLeftButton"},
            {"pad", "0 0 0 0"}}),
    detachButton(master_, this, kwargs
            {{"text", "detach"},
            {"height", "20"},
            {"width", "70"},
            {"look", "BoxRightButton"},
            {"pad", "0 0 0 0"}})
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
