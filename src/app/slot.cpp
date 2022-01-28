#include "app/slot.h"
#include "app/layout.h"
#include "app/app.h"

#include <assert.h>
#include <iostream>

namespace app {

Slot::Slot(Tab *t) :
    Box(t->get_master(), ui::Side::down, ui::Side::left, {.look = "basebox"}),
    app(*(App*)t->get_master()),
    tab(t),
    pushLeft(t->get_master(), [&](){ push_left(); }, "L"),
    pushRight(t->get_master(), [&](){ push_right(); }, "R")
{

    overload_inner(content);
    
    pushLeft.set_border(1, 0, 1, 1);
    pushLeft.text_stick(ui::Text::middle);
    pushLeft.text_offset(0, -0.3);
    pushRight.set_border(1, 0, 1, 1);
    pushRight.text_stick(ui::Text::middle);
    pushRight.text_offset(0, -0.3);
    
    buttons.insert(&pushLeft, 0);
    buttons.insert(&pushRight, 1);

    set_look(look);
}

Slot::~Slot(){
    if(content != nullptr) delete content;
}

void Slot::set_look(std::string look_){

    ui::Box::set_look(look_);

    pushLeft.set_look(chars("buttons"));
    pushRight.set_look(chars("buttons"));

    float size = num("barSize");

    pushLeft.set_target_size(size, size);
    pushRight.set_target_size(size, size);
    
    buttons.update_grid();
    buttons.fit_content();
}

void Slot::save(Saver &saver){

}

void Slot::load(Loader &loader){

}

void Slot::set_content(ui::Frame *c){
    if(content != nullptr) delete content;
    content = c;
    overload_inner(c);
}

void Slot::func_detach(){
    Box::func_detach();
    delete this;
}

ui::Frame::Capture Slot::on_event(sf::Event event, int32_t priority){
    
    if(event.type == sf::Event::MouseButtonPressed){
        select();
        return Capture::use;
    }

    return Capture::pass;
}

void Slot::select(){
    dot.fill_by_border(1);
    app.layout.select_slot(this);
}

void Slot::forget(){
    dot.fill_by_border(0);
    set_refresh();
}

bool Slot::content_from_type(std::string type){
    
    if(!valid_type(type)) return 0;

    set_content(types[type](&app));

    return 1;
}

void Slot::add_content_type(std::string type, std::function<ui::Frame*(App*)> construct){
    assert(types.count(type) == 0);
    types[type] = construct;
}

void Slot::push_left(){

    uint32_t index = app.layout.stack.find(tab);

    assert(index < app.layout.stack.size());

    if(index > 0){
        Box::func_detach();
        Tab *left = (Tab*)app.layout.stack.get(index - 1);
        tab = left;
        left->stack.push_back(set_manager(&left->stack));
    }
}

void Slot::push_right(){

    uint32_t index = app.layout.stack.find(tab);

    assert(index < app.layout.stack.size());

    if(index + 1 < app.layout.stack.size()){
        Box::func_detach();
        Tab *right = (Tab*)app.layout.stack.get(index + 1);
        tab = right;
        right->stack.push_back(set_manager(&right->stack));
    }
}

bool Slot::valid_type(std::string type){
    return types.count(type) != 0;
}

std::map<std::string, std::function<ui::Frame*(App*)> > Slot::types;

}

