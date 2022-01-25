#include "app/layout.h"
#include "app/app.h"

namespace app {

// layout /////////////////////////////////////////////////////////////////////

Layout::Layout(App *app) :
    ui::Slider(app, ui::Side::up, ui::Side::left, {.look = "basebox"}),
    addTab(app, [&](){ add_tab(); })
{
    set_border(0, 0, 0, 0);
    functions.set_border(0, 0, 0, 1);
    label.set_border(0, 0, 0, 1);
    stack.set_border(0, 0, 0, 0);
    set_slidable(0);
}

Layout::~Layout(){}

void Layout::save(Saver &saver){

}

void Layout::load(Loader &loader){

}

void Layout::forget_slot(){

}

void Layout::add_tab(){
    
}

// tab ////////////////////////////////////////////////////////////////////////

Tab::Tab(App *app) :
    Box(app, ui::Side::left, ui::Side::up, {.look = "basebox"}),
    addSlot(app, [&](){ add_slot(); })
{

}

Tab::~Tab(){}

void Tab::save(Saver &saver){

}

void Tab::load(Loader &loader){

}

void Tab::func_detach(){

}

void Tab::add_slot(){

}

// slot ///////////////////////////////////////////////////////////////////////

Slot::Slot(App *app) :
    Box(app, ui::Side::down, ui::Side::left, {.look = "basebox"}),
    pushLeft(app, [&](){ push_left(); }),
    pushRight(app, [&](){ push_right(); })
{

}

Slot::~Slot(){}

void Slot::save(Saver &saver){

}

void Slot::load(Loader &loader){

}

void Slot::set_content(Content*){

}

void Slot::func_detach(){

}

ui::Frame::Capture Slot::on_event(sf::Event event, int32_t priority){
    return Capture::pass;
}

void Slot::select(){

}

void Slot::forget(){

}

void Slot::add_content_type(std::string type, std::function<Content*(void)> construct){

}

void Slot::push_left(){

}

void Slot::push_right(){

}

Content *Slot::content_from_type(std::string type){
    return nullptr;
}

std::map<std::string, std::function<Content*(void)> > Slot::types;

// content ////////////////////////////////////////////////////////////////////

Content::Content(std::string t) : type(t) {}

Content::~Content(){}

std::string Content::get_type(){
    return type;
}

}
