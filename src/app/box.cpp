#include "app/box.h"

#include <iostream>

Box::Box(ui::Frame *parent_, std::string title_, std::map<std::string, std::string> values) :
    ResizableFrame(parent_,
            {{"look", "Box"},
            {"height", "200"},
            {"border", "0 0 2 15"},
            {"resize", "0 0 0 1"},
            {"pad", "1 1 1 1"}}),
    inner(this),
    buttonFrame(this, {{"look", "ButtonBox"}, {"height", "24"}}),
    titleText(this, 
            {{"look", "BoxTitle"},
            {"height", "24"},
            {"width", "0"},
            {"text", title_}}),
    upSwitch(this, switch_up, this,
            {{"text", "up"},
            {"height", "20"},
            {"width", "50"},
            {"look", "BoxButton"},
            {"pad", "1 1 1 1"}}),
    downSwitch(this, switch_down, this,
            {{"text", "down"},
            {"height", "20"},
            {"width", "50"},
            {"look", "BoxButton"},
            {"pad", "1 1 1 1"}}),
    upPush(this, push_up, this,
            {{"text", "UP"},
            {"height", "20"},
            {"width", "50"},
            {"look", "BoxButton"},
            {"pad", "1 1 1 1"}}),
    downPush(this, push_down, this,
            {{"text", "DOWN"},
            {"height", "20"},
            {"width", "50"},
            {"look", "BoxButton"},
            {"pad", "1 1 1 1"}}),
    xButton(this, detach_box, this,
            {{"text", "X"},
            {"height", "20"},
            {"width", "20"},
            {"look", "BoxButton"},
            {"pad", "1 1 1 1"}})
{
    
    title = title_;

    set_inner(&inner);

    inner.setup_grid(2, 1);
    inner.grid[0][0] = &buttonFrame;

    inner.fill_width({1});
    inner.fill_height({0, 1});

    buttonFrame.setup_grid(1, 10);
    buttonFrame.fill_height(0, 1);
    
    buttonFrame.fill_width(5, 1);

    buttonFrame.grid[0][0] = &upSwitch;
    buttonFrame.grid[0][1] = &downSwitch;
    buttonFrame.grid[0][2] = &upPush;
    buttonFrame.grid[0][3] = &downPush;
    buttonFrame.grid[0][5] = &titleText;
    buttonFrame.grid[0][9] = &xButton;

}

bool Box::place_to_tab(Tab *newTab){
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

int32_t Box::push_up(void *box){
    Box &x = *(Box*)box;
    x.tab->push_box(x.index(), -1);
    return 0;
}

int32_t Box::push_down(void *box){
    Box &x = *(Box*)box;
    x.tab->push_box(x.index(), 1);
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



NoContentBox::NoContentBox(ui::Frame *parent_,
        std::string title_, std::map<std::string, std::string> values) :
    Box(parent_, title_, values),
    text(this, 
            {{"look", "BoxTitle"},
            {"height", "24"},
            {"width", "80"},
            {"text", title_}})
{
    inner.grid[0][1] = &text;
}

