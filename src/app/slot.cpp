#include "app/slot.h"
#include "app/layout.h"
#include "app/app.h"
#include "app/content.h"

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
    pushRight.set_border(1, 0, 1, 1);
    
    buttons.insert(&pushLeft, 0);
    buttons.insert(&pushRight, 1);

    set_look(look);
}

Slot::~Slot(){
    reset();
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
    
    saver.write_float(targetHeight);

    if(content == nullptr){
        saver.write_unsigned(0);
    }
    else {
        saver.write_unsigned(1);
        saver.write_string(content->content_type());
        content->save(saver);
    }

}

void Slot::load(Loader &loader){
    
    reset();
    
    set_target_size(0, loader.read_float());

    if(loader.read_unsigned()){
        content_from_type(loader.read_string());
        content->load(loader);
    }
    
}

void Slot::reset(){
    if(content != nullptr) delete content;
}

void Slot::set_content(Content *c){
    if(content != nullptr) delete content;
    content = c;
    overload_inner(c);
    update_grid();
}

void Slot::func_detach(){
    Box::func_detach();
    
    if(app.layout.get_selected() == this)
        app.layout.forget_slot();    
    
    delete this;
}

ui::Frame::Capture Slot::on_event(sf::Event event, int priority){
    
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

    Content *generated = create_content(type, &app);

    if(generated == nullptr) return 0;

    set_content(generated);

    set_label(type);

    return 1;
}

void Slot::push_left(){

    unsigned index = app.layout.stack.find(tab);

    assert(index < app.layout.stack.size());

    if(index > 0){
        Box::func_detach();
        Tab *left = (Tab*)app.layout.stack.get(index - 1);
        tab = left;
        left->stack.push_back(set_manager(&left->stack));
    }
}

void Slot::push_right(){

    unsigned index = app.layout.stack.find(tab);

    assert(index < app.layout.stack.size());

    if(index + 1 < app.layout.stack.size()){
        Box::func_detach();
        Tab *right = (Tab*)app.layout.stack.get(index + 1);
        tab = right;
        right->stack.push_back(set_manager(&right->stack));
    }
}

}

