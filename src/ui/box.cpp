#include "ui/box.h"

#include <assert.h>
#include <iostream>

namespace ui {

Box::Box(Window *master_, Side barSide, Side stackStick, bool scrollable, Kwargs kwargs) :
    Slider(master_, barSide, stackStick, scrollable, kwargs),
    pushUp(master_, [&](){ func_up(); }, "U"),
    pushDown(master_, [&](){ func_down(); }, "D"),
    click(master_, [&](){ func_click(); }, "C"),
    maximize(master_, [&](){ func_maximize(); }, "O"),
    minimize(master_, [&](){ func_minimize(); }, "."),
    detach(master_, [&](){ func_detach(); }, "X")
{
    stack.set_border(0, 0, 0, 0);

    buttons.push_back(&pushUp);
    buttons.push_back(&pushDown);
    buttons.push_back(&click);
    buttons.push_back(&maximize);
    buttons.push_back(&minimize);
    buttons.push_back(&detach);

    std::array<Button*, 6> all{&pushUp, &pushDown, &click, &maximize, &minimize, &detach};

    if(side == Side::left || side == Side::right){
        label.set_border(1, 1, 0, 0);
        functions.set_border(1, 1, 0, 0);
        for(auto button : all){
            button->set_border(1, 1, 1, 0);
            button->text_direction(Text::down);
            button->text_offset(0, 0);
        }
    }
    else {
        label.set_border(0, 0, 1, 1);
        functions.set_border(0, 0, 1, 1);
        for(auto button : all){
            button->set_border(1, 0, 1, 1);
            button->text_direction(Text::left);
            button->text_offset(0, -0.1);
        }
    }

    set_look(look);
}

void Box::set_look(std::string look_){

    Slider::set_look(look_);

    pushUp.set_look(chars("buttons"));
    pushDown.set_look(chars("buttons"));
    click.set_look(chars("buttons"));
    maximize.set_look(chars("buttons"));
    minimize.set_look(chars("buttons"));
    detach.set_look(chars("buttons"));

    float size = num("barSize");

    pushUp.set_target_size(size, size);
    pushDown.set_target_size(size, size);
    click.set_target_size(size, size);
    maximize.set_target_size(size, size);
    minimize.set_target_size(size, size);
    detach.set_target_size(size, size);

    buttons.update_grid();
    buttons.fit_content();

}

void Box::on_resize(sf::Mouse::Button button, float width, float height){
    
    float size = height;
    if(side == Side::left || side == Side::right) size = width;

    if(button == sf::Mouse::Left){
        manager->push_resize(this, size);
    }
    else if(button == sf::Mouse::Right){
        manager->sticky_resize(this, size);

    }
}

Box *Box::set_stack(Stack *m){
    manager = m;
    return this;
}

void Box::func_up(){
    assert(manager != nullptr);
    manager->move(this, 1);
}

void Box::func_down(){
    assert(manager != nullptr);
    manager->move(this, -1);
}

void Box::func_click(){
    assert(manager != nullptr);
    manager->scroll_to(this);
}

void Box::func_maximize(){
    assert(manager != nullptr);
    manager->push_resize(this, manager->max_size(this));
}

void Box::func_minimize(){
    assert(manager != nullptr);
    manager->push_resize(this, 0);
}

void Box::func_detach(){
    assert(manager != nullptr);
    manager->erase(this);
    manager = nullptr;
}

}

