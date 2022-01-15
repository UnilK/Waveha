#include "app/tab.h"

#include <iostream>

namespace app {

Tab::Tab(ui::Window *master_, std::string title_, Kwargs kwargs) :
    ResizableFrame(master_, {0, 15, 0, 0}, {0, 1, 0, 0}, {.look = "Tab", .width = 500}),
    inner(master_),
    boxFrame(master_, {.look = "TabBackground"}),
    titleFrame(master_, title_, {.look = "TabTitle", .height = 20})
{

    title = title_;

    set_inner(&inner);

    inner.setup_grid(2, 1);
    inner.fill_height(1, 1);
    inner.fill_width(0, 1);

    inner.put(0, 0, &titleFrame);
    inner.put(1, 0, &boxFrame);

    boxFrame.auto_contain(0);
    boxFrame.setup_grid(boxMax, 1);
    boxFrame.fill_width(0, 1);

    ((ResizerFrame*)get(1, 2))->set_scrolled(&boxFrame);

}

bool Tab::swap_box(int32_t index, int32_t direction){
    
    int32_t other = index + direction;
    
    if(other < 0 || other >= boxMax) return 0;
    
    std::swap(boxFrame.get(index, 0), boxFrame.get(other, 0));
    clean_layout();
    boxFrame.update_grid();
    
    return 1;
}

void Tab::detach_box(int32_t index){
    boxFrame.remove_frame(index, 0);
    clean_layout();
    boxFrame.update_grid();
}

bool Tab::add_box(Box *box){
    int32_t index = 0;
    while(index < boxMax && boxFrame.get(index, 0) != nullptr) index++;
    if(index == boxMax) return 0;
    boxFrame.put(index, 0, box);
    boxFrame.update_grid();
    return 1;
}

void Tab::clean_layout(){

    // stack all of the boxes at the beginning

    for(int32_t i=0, j=0; j<boxMax; j++){
        if(boxFrame.get(j, 0) == nullptr) continue;
        while(i < j && boxFrame.get(i, 0) != nullptr) i++;
        std::swap(boxFrame.get(i, 0), boxFrame.get(j, 0));
    }
}

int32_t Tab::box_index(Box *box){
    for(int32_t i=0; i<boxMax; i++){
        if(boxFrame.get(i, 0) == box) return i;
    }

    return -1;
}

ui::Frame::Capture Tab::on_event(sf::Event event, int32_t priority){

    if(event.type == sf::Event::MouseButtonPressed){
       
        // ((MainFrame*)get_top())->chosenTab = this;

        return Capture::use;
    }

    return Capture::pass;
}

void Tab::rename(std::string name){
    title = name;
    titleFrame.set_text(title);
}

std::string Tab::get_title(){ return title; }

}
