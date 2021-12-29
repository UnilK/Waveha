#include "app/tab.h"

#include <iostream>

Tab::Tab(ui::Frame *parent_, std::string title_, std::map<std::string, std::string> values) :
    ResizableFrame(
            parent_,
            {{"look", "Tab"},
            {"width", "500"},
            {"border", "2 15 0 0"},
            {"resize", "0 1 0 0"},
            {"pad", "1 1 1 1"}}),
    inner(this),
    boxFrame(this, {{"look", "TabBackground"}}),
    titleFrame(this, {{"look", "TabTitle"}, {"height", "20"}, {"text", title_}})
{

    title = title_;

    set_inner(&inner);

    inner.setup_grid(2, 1);
    inner.fill_height(1, 1);
    inner.fill_width(0, 1);

    inner.grid[0][0] = &titleFrame;
    inner.grid[1][0] = &boxFrame;

    boxFrame.setup_grid(boxMax, 1);
    boxFrame.fill_width(0, 1);

}

bool Tab::swap_box(int32_t index, int32_t direction){
    
    int32_t other = index + direction;
    
    if(other < 0 || other >= boxMax) return 0;
    
    std::swap(boxFrame.grid[index][0], boxFrame.grid[other][0]);
    boxFrame.update_grid();
    
    return 1;
}

bool Tab::push_box(int32_t index, int32_t direction){
    
    int32_t other = index + direction;
    
    if(other < 0 || other >= boxMax) return 0;

    if(boxFrame.grid[other][0] != nullptr){
        bool possible = push_box(other, direction);
        if(!possible) return 0;
    }

    std::swap(boxFrame.grid[other][0], boxFrame.grid[index][0]);
    boxFrame.update_grid();

    return 1;
}

void Tab::detach_box(int32_t index){
    boxFrame.grid[index][0] = nullptr;
    boxFrame.update_grid();
}

bool Tab::add_box(Box *box){
    int32_t index = 0;
    while(index < boxMax && boxFrame.grid[index][0] != nullptr) index++;
    if(index == boxMax) return 0;
    boxFrame.grid[index][0] = box;
    boxFrame.update_grid();
    return 1;
}

int32_t Tab::box_index(Box *box){
    for(int32_t i=0; i<boxMax; i++) if(boxFrame.grid[i][0] == box) return i;
    return -1;
}

int32_t Tab::on_event(sf::Event event){
    return -1;
}

void Tab::rename(std::string name){
    title = name;
    titleFrame.set_text(title);
}

std::string Tab::get_title(){ return title; }

