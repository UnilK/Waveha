#include "app/tab.h"
#include "app/slot.h"
#include "app/layout.h"
#include "app/app.h"

namespace app {

Tab::Tab(App *a) :
    Box(a, ui::Side::right, ui::Side::up, {.look = "basebox"}),
    app(*a),
    addSlot(a, [&](){ add_slot(); }, "+")
{
    addSlot.set_border(1, 1, 1, 0);
    buttons.insert(&addSlot, 0);

    set_look(look);
}

Tab::~Tab(){
    reset();
}

void Tab::set_look(std::string look_){

    ui::Box::set_look(look_);

    addSlot.set_look(chars("buttons"));

    float size = num("barSize");

    addSlot.set_target_size(size, size);
    
    buttons.update_grid();
    buttons.fit_content();
}

void Tab::save(Saver &saver){

    saver.write_float(targetWidth);
    saver.write_float(canvasY);

    saver.write_unsigned(stack.size());

    for(unsigned i=0; i<stack.size(); i++){
        Tab *tab = (Tab*)stack.get(i);
        tab->save(saver);
    }
}

void Tab::load(Loader &loader){

    reset();

    set_target_size(loader.read_float(), 0);
    set_canvas_position(0, loader.read_float());

    unsigned samount = loader.read_unsigned();
    
    for(unsigned i=0; i<samount; i++){
        Slot *slot = new Slot(this);
        stack.push_back(slot->set_manager(&stack));
        slot->load(loader);
    }
}

void Tab::reset(){
    for(unsigned i=0; i<stack.size(); i++) delete stack.get(i);
    stack.clear();
}

void Tab::func_detach(){
    Box::func_detach();
    delete this;
}

void Tab::add_slot(){
    Slot *slot = new Slot(this);
    stack.push_back(slot->set_manager(&stack));
}

}

