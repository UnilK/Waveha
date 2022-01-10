#include "app/box.h"

#include <iostream>

namespace app{

Box::Box(ui::Window *master_, std::string title_, kwargs values) :
    ResizableFrame(master_,
            {{"look", "Box"},
            {"height", "200"},
            {"border", "0 0 0 15"},
            {"resize", "0 0 0 1"},
            {"pad", "0 0 0 0"}}),
    inner(this),
    buttonFrame(this, {{"look", "ButtonBox"}, {"height", "24"}}),
    titleText(this, 
            {{"look", "BoxTitle"},
            {"height", "20"},
            {"text", title_}}),
    upSwitch(this, switch_up, this,
            {{"text", "up"},
            {"height", "20"},
            {"width", "50"},
            {"look", "BoxLeftButton"},
            {"pad", "0 0 0 0"}}),
    downSwitch(this, switch_down, this,
            {{"text", "down"},
            {"height", "20"},
            {"width", "50"},
            {"look", "BoxLeftButton"},
            {"pad", "0 0 0 0"}}),
    xButton(this, detach_box, this,
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
    buttonFrame.put(0, 9, &xButton);

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

int32_t Box::switch_up(void *box){
    Box &x = *(Box*)box;
    x.tab->swap_box(x.index(), -1);
    return 0;
}

int32_t Box::switch_down(void *box){
    Box &x = *(Box*)box;
    x.tab->swap_box(x.index(), 1);
    return 0;
}

int32_t Box::detach_box(void *box){
    Box &x = *(Box*)box;
    x.detach_from_tab();
    return 0;
}

int32_t Box::index(){
    if(tab == nullptr) return -1;
    return tab->box_index(this);
}

}
